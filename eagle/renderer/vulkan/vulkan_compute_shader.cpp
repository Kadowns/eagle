//
// Created by Novak on 22/03/2020.
//

#include <eagle/renderer/vulkan/vulkan_compute_shader.h>
#include <eagle/renderer/vulkan/vulkan_shader_utils.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/file_system.h>

namespace eagle {

VulkanComputeShader::VulkanComputeShader(const std::string &path, const VulkanComputeShaderCreateInfo &createInfo)
        : m_createInfo(createInfo) {

    std::vector<uint8_t> byteCode = std::move(FileSystem::instance()->read_bytes(path));
    m_code.resize(byteCode.size() / sizeof(uint32_t));
    std::memcpy(m_code.data(), byteCode.data(), byteCode.size());

    create_pipeline_layout();
    create_pipeline();
    create_descriptor_sets();
    create_command_buffer();
    create_fence();
}

VulkanComputeShader::~VulkanComputeShader(){
    vkDestroyFence(m_createInfo.device, m_fence, nullptr);
    vkFreeCommandBuffers(m_createInfo.device, m_createInfo.commandPool, 1, &m_commandBuffer);
    cleanup_pipeline();
    vkDestroyPipelineLayout(m_createInfo.device, m_pipelineLayout, nullptr);
    clear_descriptor_set();
    m_descriptorLayout.reset();
}

void VulkanComputeShader::create_pipeline_layout() {
    EG_TRACE("eagle","BEGIN");
    std::map<uint32_t, std::map<uint32_t, DescriptorBindingDescription>> descriptorSetMap;
    std::vector<VkPushConstantRange> pushConstantsRanges;

    VulkanShaderUtils::add_bindings_from_shader_stage(m_code, VK_SHADER_STAGE_COMPUTE_BIT, descriptorSetMap,
                                                      pushConstantsRanges);

    if (descriptorSetMap.size() > 1) {
        throw std::runtime_error("More than one descriptor set on compute shader!");
    }

    for (auto &binding : descriptorSetMap[0]) {
        m_bindingDescriptions.emplace_back(binding.second);
    }


    m_descriptorLayout = std::make_shared<VulkanDescriptorSetLayout>(
            DescriptorSetLayoutCreateInfo{m_bindingDescriptions},
            VulkanDescriptorSetLayoutCreateInfo{m_createInfo.device}
            );


    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantsRanges.size();
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantsRanges.data();
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorLayout->native_layout();

    VK_CALL_ASSERT(vkCreatePipelineLayout(m_createInfo.device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout)) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    EG_TRACE("eagle","END");
}

void VulkanComputeShader::create_pipeline(){
    EG_TRACE("eagle","BEGIN");
    if (!m_cleared){
        cleanup_pipeline();
    }

    VkShaderModule shaderModule = VulkanShaderUtils::create_shader_module(m_createInfo.device, m_code);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    vertShaderStageInfo.module = shaderModule;
    vertShaderStageInfo.pName = "main";


    VkComputePipelineCreateInfo computePipelineCreateInfo = {};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.layout = m_pipelineLayout;
    computePipelineCreateInfo.stage = vertShaderStageInfo;

    VK_CALL_ASSERT(vkCreateComputePipelines(m_createInfo.device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &m_computePipeline)){
        throw std::runtime_error("Failed to create compute pipeline!");
    }

    vkDestroyShaderModule(m_createInfo.device, shaderModule, nullptr);
    m_cleared = false;
    EG_TRACE("eagle","END");
}

void VulkanComputeShader::create_descriptor_sets() {
    EG_TRACE("eagle","BEGIN");

    DescriptorSetCreateInfo descriptorSetInfo = {};
    descriptorSetInfo.layout = m_descriptorLayout;

    VulkanDescriptorSetInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.device = m_createInfo.device;
    descriptorSetCreateInfo.frameCount = m_createInfo.bufferCount;

    m_descriptorSet = std::make_shared<VulkanDescriptorSet>(descriptorSetInfo, descriptorSetCreateInfo);
    EG_TRACE("eagle","END");
}


void VulkanComputeShader::create_command_buffer() {
    EG_TRACE("eagle","BEGIN");
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_createInfo.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CALL_ASSERT(vkAllocateCommandBuffers(m_createInfo.device, &allocInfo, &m_commandBuffer)) {
        throw std::runtime_error("failed to allocate compute shader command buffer!");
    }
    EG_TRACE("eagle","END");
}

void VulkanComputeShader::create_fence() {
    EG_TRACE("eagle","BEGIN");
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateFence(m_createInfo.device, &fenceCreateInfo, nullptr, &m_fence);
    EG_TRACE("eagle","END");
}

void VulkanComputeShader::cleanup_pipeline(){
    EG_TRACE("eagle","BEGIN");
    if (m_cleared){
        EG_WARNING("eagle", "END - ALREADY CLEARED");
        return;
    }
    vkDestroyPipeline(m_createInfo.device, m_computePipeline, nullptr);

    m_cleared = true;
    EG_TRACE("eagle","BEGIN");
}

void VulkanComputeShader::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {

    vkQueueWaitIdle(m_createInfo.computeQueue);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(m_commandBuffer, &beginInfo);

    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline);

    if (!m_pushConstantData.empty()){
        vkCmdPushConstants(m_commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, m_pushConstantData.size(), m_pushConstantData.data());
    }

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout, 0, 1, &m_descriptorSet->get_descriptors()[*m_createInfo.imageIndex], 0, nullptr);

    vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);

    vkEndCommandBuffer(m_commandBuffer);

    vkWaitForFences(m_createInfo.device, 1, &m_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(m_createInfo.device, 1, &m_fence);

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &m_commandBuffer;

    VK_CALL_ASSERT(vkQueueSubmit(m_createInfo.computeQueue, 1, &computeSubmitInfo, m_fence)){
        throw std::runtime_error("Failed to submit compute command buffer!");
    }

}

void VulkanComputeShader::clear_descriptor_set() {
    m_descriptorSet.reset();
}

void VulkanComputeShader::recreate(uint32_t bufferCount) {
    m_createInfo.bufferCount = bufferCount;
    create_descriptor_sets();
}

void VulkanComputeShader::update_image(uint32_t binding, const std::shared_ptr<ImageView>& image) {
    m_descriptorSet->operator[](binding) = image;
}

void VulkanComputeShader::join() {
    vkQueueWaitIdle(m_createInfo.computeQueue);
}

void VulkanComputeShader::update_descriptors() {
    m_descriptorSet->flush_all();
}

void VulkanComputeShader::update_push_constants(void* data, size_t size) {
    m_pushConstantData.resize(size);
    memcpy(m_pushConstantData.data(), data, size);
}


}