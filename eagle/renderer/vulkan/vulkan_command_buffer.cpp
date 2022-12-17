//
// Created by Novak on 27/09/2019.
//

#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_vertex_buffer.h>
#include <eagle/renderer/vulkan/vulkan_index_buffer.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set.h>
#include <eagle/renderer/vulkan/vulkan_render_pass.h>
#include <eagle/renderer/vulkan/vulkan_framebuffer.h>

namespace eagle {

namespace detail{

static std::vector<VkCommandBuffer> native_command_buffers(const std::span<CommandBuffer*>& commandBuffers){
    std::vector<VkCommandBuffer> result;
    result.reserve(commandBuffers.size());
    for (auto commandBuffer : commandBuffers){
        auto castedCommandBuffer = (VulkanCommandBuffer*)commandBuffer;
        result.push_back(castedCommandBuffer->current_command_buffer());
    }
    return result;
}

}

VulkanCommandBuffer::VulkanCommandBuffer(const CommandBufferCreateInfo &createInfo,
                                         const VulkanCommandBufferCreateInfo &vkCreateInfo) :
    CommandBuffer(createInfo),
    m_nativeCreateInfo(vkCreateInfo){

}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    auto castedQueue = (VulkanCommandQueue*)m_createInfo.queue;
    if (m_currentCommandBuffer != VK_NULL_HANDLE) {
        castedQueue->free(m_currentCommandBuffer, m_currentThreadId);
    }
}

void VulkanCommandBuffer::begin() {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);

    m_currentFrame = *m_nativeCreateInfo.currentFrame;
    prepare_command_buffer();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    vkBeginCommandBuffer(m_currentCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::begin(RenderPass* renderPass,
                                Framebuffer* framebuffer) {
    assert(m_createInfo.level == CommandBufferLevel::SECONDARY);

    m_currentFrame = *m_nativeCreateInfo.currentFrame;
    prepare_command_buffer();

    auto vrp = (VulkanRenderPass*)renderPass;
    auto vfb = (VulkanFramebuffer*)framebuffer;

    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.occlusionQueryEnable = VK_FALSE;
    inheritanceInfo.renderPass = vrp->native_render_pass();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = &inheritanceInfo;

    inheritanceInfo.framebuffer = vfb->current_native_framebuffer();
    vkBeginCommandBuffer(m_currentCommandBuffer, &beginInfo);
}

void VulkanCommandBuffer::end() {
    vkEndCommandBuffer(m_currentCommandBuffer);
    m_boundShader = nullptr;
}

void VulkanCommandBuffer::execute_commands(const std::span<CommandBuffer*>& commandBuffers) {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY);
    auto nativeCommandBuffers = detail::native_command_buffers(commandBuffers);

    vkCmdExecuteCommands(
            m_currentCommandBuffer,
            nativeCommandBuffers.size(),
            nativeCommandBuffers.data());
}

void VulkanCommandBuffer::begin_render_pass(RenderPass* renderPass,
                                            Framebuffer* framebuffer) {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);
    auto vrp = (VulkanRenderPass*)renderPass;
    auto vfb = (VulkanFramebuffer*)framebuffer;

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vrp->native_render_pass();
    renderPassInfo.framebuffer = vfb->current_native_framebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vfb->width(), vfb->height()};
    auto& clearValues = vrp->clear_values();
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    VkSubpassContents contents = m_createInfo.level == CommandBufferLevel::MASTER
            ? VK_SUBPASS_CONTENTS_INLINE
            : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

    vkCmdBeginRenderPass(m_currentCommandBuffer, &renderPassInfo, contents);

}

void VulkanCommandBuffer::end_render_pass() {
    assert(m_createInfo.level == CommandBufferLevel::PRIMARY || m_createInfo.level == CommandBufferLevel::MASTER);

    vkCmdEndRenderPass(m_currentCommandBuffer);
}

void VulkanCommandBuffer::bind_shader(Shader* shader) {
    m_boundShader = (VulkanShader*)shader;
    vkCmdBindPipeline(m_currentCommandBuffer, m_boundShader->native_bind_point(), m_boundShader->native_pipeline());
}

void VulkanCommandBuffer::bind_vertex_buffer(VertexBuffer* vertexBuffer, uint32_t binding) {
    auto vvb = (VulkanVertexBuffer*)vertexBuffer;
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(
            m_currentCommandBuffer,
            binding,
            1,
            &vvb->internal_buffer(m_currentFrame).native_buffer(),
            offsets);
}

void VulkanCommandBuffer::bind_index_buffer(IndexBuffer* indexBuffer) {
    auto vib = (VulkanIndexBuffer*)indexBuffer;

    vkCmdBindIndexBuffer(
            m_currentCommandBuffer,
            vib->internal_buffer(m_currentFrame).native_buffer(),
            0,
            vib->native_index_type());
}

void VulkanCommandBuffer::bind_descriptor_sets(DescriptorSet* descriptorSet, uint32_t setIndex) {
    auto vds = (VulkanDescriptorSet*)descriptorSet;

    VkDescriptorSet descriptorSets[] = {vds->native_descriptor_set(m_currentFrame)};

    vkCmdBindDescriptorSets(
            m_currentCommandBuffer,
            m_boundShader->native_bind_point(),
            m_boundShader->native_pipeline_layout(),
            setIndex,
            1,
            descriptorSets,
            0,
            nullptr);
}

void VulkanCommandBuffer::push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) {

    vkCmdPushConstants(
            m_currentCommandBuffer,
            m_boundShader->native_pipeline_layout(),
            VulkanConverter::to_vk(stage),
            offset,
            size,
            data);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount) {
    vkCmdDraw(m_currentCommandBuffer, vertexCount, 1, 0, 0);
}

void VulkanCommandBuffer::draw_indexed(uint32_t indicesCount, uint32_t instanceCount, uint32_t firstIndex,
                                       uint32_t vertexOffset, uint32_t firstInstance) {
    vkCmdDrawIndexed(m_currentCommandBuffer, indicesCount, instanceCount, firstIndex, (int32_t)vertexOffset, firstInstance);
}

void VulkanCommandBuffer::set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) {
    VkViewport viewport ={};
    viewport.width = w;
    viewport.height = h;
    viewport.x = x;
    viewport.y = y;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;

    vkCmdSetViewport(m_currentCommandBuffer, 0, 1, &viewport);
}

void VulkanCommandBuffer::set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
    VkRect2D scissor = {};
    scissor.extent = {w, h};
    scissor.offset = {static_cast<int32_t>(x), static_cast<int32_t>(y)};

    vkCmdSetScissor(m_currentCommandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::pipeline_barrier(std::span<ImageMemoryBarrier> imageMemoryBarriers, PipelineStageFlags srcPipelineStages,
                                           PipelineStageFlags dstPipelineStages) {

    std::vector<VkImageMemoryBarrier> nativeImageMemoryBarriers;
    nativeImageMemoryBarriers.reserve(imageMemoryBarriers.size());

    for (auto& imageMemoryBarrier : imageMemoryBarriers){
        auto castedImage = (VulkanImage*)imageMemoryBarrier.image;

        for (int i = 0; (i < castedImage->frame_count() && imageMemoryBarrier.allFrames) || i < 1; i++){
            VkImageMemoryBarrier nativeImageMemoryBarrier = {};
            nativeImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

            nativeImageMemoryBarrier.oldLayout = VulkanConverter::to_vk(imageMemoryBarrier.oldLayout);
            nativeImageMemoryBarrier.newLayout = VulkanConverter::to_vk(imageMemoryBarrier.newLayout);

            nativeImageMemoryBarrier.srcAccessMask = VulkanConverter::eg_flags_to_vk_flags<AccessFlagBits>(imageMemoryBarrier.srcAccessMask);
            nativeImageMemoryBarrier.dstAccessMask = VulkanConverter::eg_flags_to_vk_flags<AccessFlagBits>(imageMemoryBarrier.dstAccessMask);

            nativeImageMemoryBarrier.srcQueueFamilyIndex = ((VulkanCommandQueue*)imageMemoryBarrier.srcQueue)->family_index();
            nativeImageMemoryBarrier.dstQueueFamilyIndex = ((VulkanCommandQueue*)imageMemoryBarrier.dstQueue)->family_index();

            nativeImageMemoryBarrier.subresourceRange.aspectMask = VulkanConverter::eg_flags_to_vk_flags<ImageAspect>(castedImage->aspects());
            nativeImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
            nativeImageMemoryBarrier.subresourceRange.layerCount = castedImage->array_layers();
            nativeImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
            nativeImageMemoryBarrier.subresourceRange.levelCount = castedImage->mip_levels();
            nativeImageMemoryBarrier.image = castedImage->native_image(imageMemoryBarrier.allFrames ? i : m_currentFrame);

            nativeImageMemoryBarriers.push_back(nativeImageMemoryBarrier);
        }
    }

    vkCmdPipelineBarrier(
            m_currentCommandBuffer,
            VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(srcPipelineStages),
            VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(dstPipelineStages),
            0,
            0, nullptr,
            0, nullptr,
            nativeImageMemoryBarriers.size(), nativeImageMemoryBarriers.data());
}

void VulkanCommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    vkCmdDispatch(m_currentCommandBuffer, groupCountX, groupCountY, groupCountZ);
}

VkCommandBuffer VulkanCommandBuffer::current_command_buffer() {
    return m_currentCommandBuffer;
}

void VulkanCommandBuffer::prepare_command_buffer() {

    auto castedQueue = (VulkanCommandQueue*)m_createInfo.queue;

    //free old command buffer
    if (m_currentCommandBuffer){
        castedQueue->free(m_currentCommandBuffer, m_currentThreadId);
    }

    //allocate a new one for this thread
    castedQueue->allocate(m_currentCommandBuffer, VulkanConverter::to_vk(m_createInfo.level));
    m_currentThreadId = std::this_thread::get_id();
}
}