//
// Created by Novak on 22/03/2020.
//

#include <eagle/core/renderer/vulkan/VulkanComputeShader.h>
#include <eagle/core/renderer/vulkan/VulkanShaderCompiler.h>
#include <eagle/core/renderer/vulkan/VulkanShaderUtils.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>

EG_BEGIN

VulkanComputeShader::VulkanComputeShader(const std::string &path, const VulkanComputeShaderCreateInfo &createInfo)
        : m_createInfo(createInfo) {

    m_code = VulkanShaderCompiler::compile_glsl(path, ShaderStage::COMPUTE);

    create_pipeline_layout();
    create_pipeline();
    create_descriptor_sets();
    create_command_buffer();
    create_fence();
}

VulkanComputeShader::~VulkanComputeShader(){
    VK_CALL vkDestroyFence(m_createInfo.device, m_fence, nullptr);
    VK_CALL vkFreeCommandBuffers(m_createInfo.device, m_createInfo.commandPool, 1, &m_commandBuffer);
    cleanup_pipeline();
    VK_CALL vkDestroyPipelineLayout(m_createInfo.device, m_pipelineLayout, nullptr);
    clear_descriptor_set();
    m_descriptorLayout.reset();
}

void VulkanComputeShader::create_pipeline_layout() {

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

    m_descriptorLayout = std::make_shared<VulkanDescriptorSetLayout>(m_createInfo.device, m_bindingDescriptions);

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantsRanges.size();
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantsRanges.data();
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorLayout->get_native_layout();

    VK_CALL_ASSERT(vkCreatePipelineLayout(m_createInfo.device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout)) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void VulkanComputeShader::create_pipeline(){

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

    VK_CALL vkDestroyShaderModule(m_createInfo.device, shaderModule, nullptr);
    m_cleared = false;
}

void VulkanComputeShader::create_descriptor_sets() {
    VulkanDescriptorSetCreateInfo descriptorSetCreateInfo = {};
    descriptorSetCreateInfo.device = m_createInfo.device;
    descriptorSetCreateInfo.bufferCount = m_createInfo.bufferCount;
    m_descriptorSet = std::make_shared<VulkanDescriptorSet>(m_descriptorLayout, descriptorSetCreateInfo);
}


void VulkanComputeShader::create_command_buffer() {

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_createInfo.commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CALL_ASSERT(vkAllocateCommandBuffers(m_createInfo.device, &allocInfo, &m_commandBuffer)) {
        throw std::runtime_error("failed to allocate compute shader command buffer!");
    }
}

void VulkanComputeShader::create_fence() {

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CALL vkCreateFence(m_createInfo.device, &fenceCreateInfo, nullptr, &m_fence);
}

void VulkanComputeShader::cleanup_pipeline(){
    if (m_cleared){
        return;
    }
    VK_CALL vkDestroyPipeline(m_createInfo.device, m_computePipeline, nullptr);

    m_cleared = true;
}

void VulkanComputeShader::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {

    VK_CALL vkQueueWaitIdle(m_createInfo.computeQueue);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL vkBeginCommandBuffer(m_commandBuffer, &beginInfo);

    VK_CALL vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_computePipeline);

    VK_CALL vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipelineLayout, 0, 1, &m_descriptorSet->get_descriptors()[*m_createInfo.imageIndex], 0, nullptr);

    VK_CALL vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);

    VK_CALL vkEndCommandBuffer(m_commandBuffer);

    VK_CALL vkWaitForFences(m_createInfo.device, 1, &m_fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    VK_CALL vkResetFences(m_createInfo.device, 1, &m_fence);

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

void
VulkanComputeShader::update_descriptor_items(const std::vector<Reference<DescriptorItem>> &descriptorItems) {
    m_descriptorSet->update(descriptorItems);
}

void
VulkanComputeShader::set_image(const std::string &name, const Reference<Image> &image) {

}




EG_END