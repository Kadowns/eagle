#include <utility>

//
// Created by Novak on 27/09/2019.
//

#include <eagle/core/renderer/vulkan/VulkanCommandBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>
#include <eagle/core/renderer/vulkan/VulkanShader.h>
#include <eagle/core/renderer/vulkan/VulkanVertexBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanIndexBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanDescriptorSet.h>
#include <eagle/core/renderer/vulkan/VulkanComputeShader.h>
#include <eagle/core/renderer/vulkan/VulkanRenderPass.h>
#include <eagle/core/renderer/vulkan/VulkanFramebuffer.h>

EG_BEGIN

VulkanCommandBuffer::VulkanCommandBuffer(VkDevice &device, VkCommandPool &commandPool, uint32_t &imageIndexRef,
                                         const std::function<void(VkCommandBuffer&)>& submit_callback) :
    m_device(device), m_commandPool(commandPool), m_imageIndexRef(imageIndexRef), submit_command_buffer_callback(submit_callback){
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VK_CALL_ASSERT(vkAllocateCommandBuffers(device, &allocInfo, &m_commandBuffer)) {
        throw std::runtime_error("failed to allocate command buffer!");
    }
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
//    if (!m_finished){
//        VK_CALL vkEndCommandBuffer(m_commandBuffer);
//    }

    VK_CALL vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_commandBuffer);
}

void VulkanCommandBuffer::begin() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
    m_finished = false;
}

void VulkanCommandBuffer::finish() {
    VK_CALL vkEndCommandBuffer(m_commandBuffer);
    m_finished = true;
    m_boundShader.reset();
}

void VulkanCommandBuffer::submit() {
    submit_command_buffer_callback(m_commandBuffer);
}

bool VulkanCommandBuffer::is_finished() {
    return m_finished;
}

void VulkanCommandBuffer::begin_render_pass(const Reference<RenderPass> &renderPass,
                                            const Reference<Framebuffer> &framebuffer) {
    auto vrp = std::static_pointer_cast<VulkanRenderPass>(renderPass);
    auto vf = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vrp->native_render_pass();
    renderPassInfo.framebuffer = vf->native_framebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = {vf->width(), vf->height()};
    auto& clearValues = vrp->clear_values();
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    VK_CALL vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    VK_CALL vkCmdEndRenderPass(m_commandBuffer);
}

void VulkanCommandBuffer::bind_shader(const Reference<Shader> &shader) {
    m_boundShader = std::static_pointer_cast<VulkanShader>(shader);
    VK_CALL vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_boundShader->get_pipeline());
}

void VulkanCommandBuffer::bind_compute_shader(const Reference<ComputeShader> &shader) {
    Reference<VulkanComputeShader> vcs = std::static_pointer_cast<VulkanComputeShader>(shader);
    VK_CALL vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vcs->get_pipeline());
}

void VulkanCommandBuffer::bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) {
    Reference<VulkanVertexBuffer> vvb = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
    VkDeviceSize offsets[] = {0};
    VK_CALL vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &vvb->get_buffer(m_imageIndexRef).native_buffer(), offsets);
}

void VulkanCommandBuffer::bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) {
    Reference<VulkanIndexBuffer> vib = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);
    VK_CALL vkCmdBindIndexBuffer(m_commandBuffer, vib->get_buffer(m_imageIndexRef).native_buffer(), 0, vib->get_native_index_type());
}

void VulkanCommandBuffer::bind_descriptor_sets(const Reference <DescriptorSet> &descriptorSet, uint32_t setIndex) {
    Reference<VulkanDescriptorSet> vds = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VK_CALL vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_boundShader->get_layout(), setIndex, 1, &vds->get_descriptors()[m_imageIndexRef], 0, nullptr);
}

void VulkanCommandBuffer::bind_descriptor_sets(const Reference<ComputeShader> &shader, const Reference<DescriptorSet> &descriptorSet, uint32_t setIndex) {
    Reference<VulkanComputeShader> vcs = std::static_pointer_cast<VulkanComputeShader>(shader);
    Reference<VulkanDescriptorSet> vds = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VK_CALL vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vcs->get_layout(), setIndex, 1, &vds->get_descriptors()[m_imageIndexRef], 0, nullptr);
}

void VulkanCommandBuffer::push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) {
    VK_CALL vkCmdPushConstants(m_commandBuffer, m_boundShader->get_layout(), VulkanConverter::to_vk(stage), offset, size, data);
}

void VulkanCommandBuffer::draw(uint32_t vertexCount) {
    VK_CALL vkCmdDraw(m_commandBuffer, vertexCount, 1, 0, 0);
}

void VulkanCommandBuffer::draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) {
    VK_CALL vkCmdDrawIndexed(m_commandBuffer, indicesCount, 1, indexOffset, vertexOffset, 0);
}

void VulkanCommandBuffer::set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) {
    VkViewport viewport ={};
    viewport.width = w;
    viewport.height = h;
    viewport.x = x;
    viewport.y = y;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    VK_CALL vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);
}

void VulkanCommandBuffer::set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
    VkRect2D scissor = {};
    scissor.extent = {w, h};
    scissor.offset = {static_cast<int32_t>(x), static_cast<int32_t>(y)};
    VK_CALL vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::pipeline_barrier(const Reference<Image> &image, ShaderStage srcStage,
                                           ShaderStage dstStage) {
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // We won't be changing the layout of the image
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    imageMemoryBarrier.image = std::static_pointer_cast<VulkanImage>(image)->native_image();
    imageMemoryBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    VK_CALL vkCmdPipelineBarrier(
            m_commandBuffer,
            VulkanConverter::to_vk(srcStage),
            VulkanConverter::to_vk(dstStage),
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);

}

void VulkanCommandBuffer::dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) {
    VK_CALL vkCmdDispatch(m_commandBuffer, groupCountX, groupCountY, groupCountZ);
}

EG_END