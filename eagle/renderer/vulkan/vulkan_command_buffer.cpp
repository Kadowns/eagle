//
// Created by Novak on 27/09/2019.
//

#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_vertex_buffer.h>
#include <eagle/renderer/vulkan/vulkan_index_buffer.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set.h>
#include <eagle/renderer/vulkan/vulkan_compute_shader.h>
#include <eagle/renderer/vulkan/vulkan_render_pass.h>
#include <eagle/renderer/vulkan/vulkan_framebuffer.h>

namespace eagle {

VulkanCommandBuffer::VulkanCommandBuffer(const CommandBufferCreateInfo &createInfo,
                                         const VulkanCommandBufferCreateInfo &vkCreateInfo) :
                                         CommandBuffer(createInfo),
                                         m_vkCreateInfo(vkCreateInfo) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkCreateInfo.queueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL_ASSERT(vkCreateCommandPool(vkCreateInfo.device, &poolInfo, nullptr, &m_commandPool)) {
        throw std::runtime_error("failed to create graphics command pool!");
    }

   recreate(m_vkCreateInfo.imageCount);
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    cleanup();

    VK_CALL vkDestroyCommandPool(m_vkCreateInfo.device, m_commandPool, nullptr);
}

void VulkanCommandBuffer::begin() {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL vkBeginCommandBuffer(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], &beginInfo);

    m_finished = false;
}

void VulkanCommandBuffer::begin(const std::shared_ptr<RenderPass> &renderPass,
                                const std::shared_ptr<Framebuffer> &framebuffer) {
    assert(m_createInfo.level == CommandBufferLevel::SECONDARY);
    auto vrp = std::static_pointer_cast<VulkanRenderPass>(renderPass);
    auto vfb = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);

    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.occlusionQueryEnable = VK_FALSE;
    inheritanceInfo.renderPass = vrp->native_render_pass();


    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = &inheritanceInfo;

    inheritanceInfo.framebuffer = vfb->native_framebuffers()[*m_vkCreateInfo.currentImageIndex];
    VK_CALL vkBeginCommandBuffer(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], &beginInfo);
    m_finished = false;
}


void VulkanCommandBuffer::end() {
    VK_CALL vkEndCommandBuffer(m_commandBuffers[*m_vkCreateInfo.currentImageIndex]);

    m_finished = true;
    m_boundShader.reset();
}

void VulkanCommandBuffer::execute_commands(const std::vector<std::shared_ptr<CommandBuffer>> &commandBuffers) {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY);
    std::vector<VkCommandBuffer> vkCommandBuffers;
    vkCommandBuffers.reserve(commandBuffers.size());


    for (auto& commandBuffer : commandBuffers){
        vkCommandBuffers.emplace_back(((VulkanCommandBuffer*)(commandBuffer.get()))->native_command_buffers()[*m_vkCreateInfo.currentImageIndex]);
    }

    VK_CALL vkCmdExecuteCommands(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            vkCommandBuffers.size(),
            vkCommandBuffers.data());
}

void VulkanCommandBuffer::begin_render_pass(const std::shared_ptr<RenderPass> &renderPass,
                                            const std::shared_ptr<Framebuffer> &framebuffer) {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);
    auto vrp = std::static_pointer_cast<VulkanRenderPass>(renderPass);
    auto vfb = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vrp->native_render_pass();
    renderPassInfo.framebuffer = vfb->native_framebuffers()[*m_vkCreateInfo.currentImageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vfb->width(), vfb->height()};
    auto& clearValues = vrp->clear_values();
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    VkSubpassContents contents = m_createInfo.level == CommandBufferLevel::MASTER
            ? VK_SUBPASS_CONTENTS_INLINE
            : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

    VK_CALL vkCmdBeginRenderPass(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], &renderPassInfo, contents);

}

void VulkanCommandBuffer::end_render_pass() {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);

    VK_CALL vkCmdEndRenderPass(m_commandBuffers[*m_vkCreateInfo.currentImageIndex]);
}

void VulkanCommandBuffer::bind_shader(const std::shared_ptr<Shader> &shader) {
    m_boundShader = std::static_pointer_cast<VulkanShader>(shader);
    VK_CALL vkCmdBindPipeline(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_boundShader->get_pipeline());
}

void VulkanCommandBuffer::bind_compute_shader(const std::shared_ptr<ComputeShader> &shader) {
    auto vcs = std::static_pointer_cast<VulkanComputeShader>(shader);
    VK_CALL vkCmdBindPipeline(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], VK_PIPELINE_BIND_POINT_COMPUTE, vcs->get_pipeline());
}

void VulkanCommandBuffer::bind_vertex_buffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, uint32_t binding) {
    auto vvb = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
    VkDeviceSize offsets[] = {0};

    VK_CALL vkCmdBindVertexBuffers(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            binding,
            1,
            &vvb->native_buffer(*m_vkCreateInfo.currentImageIndex).native_buffer(),
            offsets);
}

void VulkanCommandBuffer::bind_index_buffer(const std::shared_ptr<IndexBuffer> &indexBuffer) {
    auto vib = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);

    VK_CALL vkCmdBindIndexBuffer(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            vib->native_buffer(*m_vkCreateInfo.currentImageIndex).native_buffer(),
            0,
            vib->native_index_type());
}

void VulkanCommandBuffer::bind_descriptor_sets(const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) {
    auto vds = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VK_CALL vkCmdBindDescriptorSets(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_boundShader->get_layout(),
            setIndex,
            1,
            &vds->get_descriptors()[*m_vkCreateInfo.currentImageIndex],
            0,
            nullptr);
}

void VulkanCommandBuffer::bind_descriptor_sets(const std::shared_ptr<ComputeShader> &shader, const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) {
    auto vcs = std::static_pointer_cast<VulkanComputeShader>(shader);
    auto vds = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VK_CALL vkCmdBindDescriptorSets(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            VK_PIPELINE_BIND_POINT_COMPUTE,
            vcs->get_layout(),
            setIndex,
            1,
            &vds->get_descriptors()[*m_vkCreateInfo.currentImageIndex],
            0,
            nullptr);
}

void VulkanCommandBuffer::push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) {

    VK_CALL vkCmdPushConstants(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            m_boundShader->get_layout(),
            VulkanConverter::to_vk(stage),
            offset,
            size,
            data);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount) {
    VK_CALL vkCmdDraw(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], vertexCount, 1, 0, 0);
}

void VulkanCommandBuffer::draw_indexed(uint32_t indicesCount, uint32_t instanceCount, uint32_t firstIndex,
                                       uint32_t vertexOffset, uint32_t firstInstance) {
    VK_CALL vkCmdDrawIndexed(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], indicesCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) {
    VkViewport viewport ={};
    viewport.width = w;
    viewport.height = h;
    viewport.x = x;
    viewport.y = y;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;

    VK_CALL vkCmdSetViewport(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], 0, 1, &viewport);
}

void VulkanCommandBuffer::set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
    VkRect2D scissor = {};
    scissor.extent = {w, h};
    scissor.offset = {static_cast<int32_t>(x), static_cast<int32_t>(y)};

    VK_CALL vkCmdSetScissor(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], 0, 1, &scissor);
}

void VulkanCommandBuffer::pipeline_barrier(const std::shared_ptr<Image> &image, const std::vector<PipelineStageFlagsBits> &srcPipelineStages,
                                           const std::vector<PipelineStageFlagsBits> &dstPipelineStages) {
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // We won't be changing the layout of the image
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    imageMemoryBarrier.srcQueueFamilyIndex = 0;
    imageMemoryBarrier.dstQueueFamilyIndex = 0;

    auto vkImage = std::static_pointer_cast<VulkanImage>(image);

    imageMemoryBarrier.image = vkImage->native_image(*m_vkCreateInfo.currentImageIndex);
    VK_CALL vkCmdPipelineBarrier(
            m_commandBuffers[*m_vkCreateInfo.currentImageIndex],
            VulkanConverter::to_vk_flags<VkPipelineStageFlags>(srcPipelineStages),
            VulkanConverter::to_vk_flags<VkPipelineStageFlags>(dstPipelineStages),
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);
}

void VulkanCommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    VK_CALL vkCmdDispatch(m_commandBuffers[*m_vkCreateInfo.currentImageIndex], groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::cleanup() {
    if (m_cleared){
        return;
    }
    VK_CALL vkFreeCommandBuffers(m_vkCreateInfo.device, m_commandPool, m_vkCreateInfo.imageCount, m_commandBuffers.data());
    m_cleared = true;
}

void VulkanCommandBuffer::recreate(uint32_t imageCount) {
    m_vkCreateInfo.imageCount = imageCount;
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VulkanConverter::to_vk(m_createInfo.level);
    allocInfo.commandBufferCount = imageCount;

    m_commandBuffers.resize(imageCount);

    VK_CALL_ASSERT(vkAllocateCommandBuffers(m_vkCreateInfo.device, &allocInfo, m_commandBuffers.data())) {
        throw std::runtime_error("failed to allocate command buffer!");
    }
    m_cleared = false;
}

}