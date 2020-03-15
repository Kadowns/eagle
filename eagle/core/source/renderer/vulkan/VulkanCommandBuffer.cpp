//
// Created by Novak on 27/09/2019.
//

#include <eagle/core/renderer/vulkan/VulkanCommandBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanConversor.h>
#include <eagle/core/renderer/vulkan/VulkanShader.h>
#include <eagle/core/renderer/vulkan/VulkanVertexBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanIndexBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanDescriptorSet.h>

EG_BEGIN

VulkanCommandBuffer::VulkanCommandBuffer(VkCommandBuffer &commandBuffer, uint32_t imageIndex) :
    m_commandBuffer(commandBuffer), m_imageIndex(imageIndex), m_finished(false) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL vkBeginCommandBuffer(m_commandBuffer, &beginInfo);
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    if (!m_finished){
        VK_CALL vkEndCommandBuffer(m_commandBuffer);
    }
}

void VulkanCommandBuffer::finish() {
    VK_CALL vkEndCommandBuffer(m_commandBuffer);
    m_finished = true;
}

bool VulkanCommandBuffer::is_finished() {
    return m_finished;
}

void VulkanCommandBuffer::begin_render_pass(const Reference<RenderTarget> &renderTarget) {
    auto vrt = std::static_pointer_cast<VulkanRenderTarget>(renderTarget);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vrt->get_render_pass();
    renderPassInfo.framebuffer = vrt->get_framebuffer();
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vrt->get_extent();
    auto clearValues = vrt->get_clear_values();
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();

    VK_CALL vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::end_render_pass() {
    VK_CALL vkCmdEndRenderPass(m_commandBuffer);
}

void VulkanCommandBuffer::bind_shader(const Reference<Shader> &shader) {
    Reference<VulkanShader> vs = std::static_pointer_cast<VulkanShader>(shader);
    VK_CALL vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vs->get_pipeline());
}

void VulkanCommandBuffer::bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) {
    Reference<VulkanVertexBuffer> vvb = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
    VkDeviceSize offsets[] = {0};
    VK_CALL vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &vvb->get_buffer(m_imageIndex).get_native_buffer(), offsets);
}

void VulkanCommandBuffer::bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) {
    Reference<VulkanIndexBuffer> vib = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);
    VK_CALL vkCmdBindIndexBuffer(m_commandBuffer, vib->get_buffer(m_imageIndex).get_native_buffer(), 0, vib->get_native_index_type());
}

void VulkanCommandBuffer::bind_descriptor_sets(const Reference<Shader> &shader, const Reference<DescriptorSet> &descriptorSet, uint32_t setIndex) {

    Reference<VulkanShader> vs = std::static_pointer_cast<VulkanShader>(shader);
    Reference<VulkanDescriptorSet> vds = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VK_CALL vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vs->get_layout(), setIndex, 1, &vds->get_descriptors()[m_imageIndex], 0, nullptr);
}

void VulkanCommandBuffer::push_constants(const Reference<Shader> &shader, ShaderStage stage, uint32_t offset, size_t size, void *data) {
    Reference<VulkanShader> vs = std::static_pointer_cast<VulkanShader>(shader);
    VK_CALL vkCmdPushConstants(m_commandBuffer, vs->get_layout(), VulkanConversor::to_vk(stage), offset, size, data);
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


EG_END