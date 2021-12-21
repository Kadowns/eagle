//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include "vulkan_global_definitions.h"
#include "vulkan_shader.h"

#include <eagle/renderer/command_buffer.h>

namespace eagle {

struct VulkanCommandBufferCreateInfo {
    VkDevice device;
    uint32_t queueFamilyIndex;
    uint32_t imageCount;
    uint32_t* currentImageIndex = nullptr;
};

class VulkanCommandBuffer : public CommandBuffer {
public:

    VulkanCommandBuffer(const CommandBufferCreateInfo& createInfo, const VulkanCommandBufferCreateInfo& vkCreateInfo);
    virtual ~VulkanCommandBuffer();

    void begin() override;
    void begin(const WeakPointer<RenderPass>& renderPass, const WeakPointer<Framebuffer>& framebuffer) override;
    void end() override;
    void execute_commands(const std::vector<WeakPointer<CommandBuffer>>& commandBuffers) override;

    void begin_render_pass(const WeakPointer<RenderPass> &renderPass, const WeakPointer<Framebuffer>& framebuffer) override;
    void end_render_pass() override;
    void bind_shader(const WeakPointer<Shader> &shader) override;
    void bind_vertex_buffer(const WeakPointer<VertexBuffer>& vertexBuffer, uint32_t binding) override;
    void bind_index_buffer(const WeakPointer<IndexBuffer> &indexBuffer) override;
    void push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    void bind_descriptor_sets(const WeakPointer<DescriptorSet> &descriptorSet, uint32_t setIndex) override;
    void draw(uint32_t vertexCount) override;
    void draw_indexed(uint32_t indicesCount, uint32_t instanceCount, uint32_t firstIndex,
                      uint32_t vertexOffset, uint32_t firstInstance) override;
    void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;
    void pipeline_barrier(const WeakPointer<Image> &image, const std::vector<PipelineStageFlagsBits> &srcPipelineStages, const std::vector<PipelineStageFlagsBits> &dstPipelineStages) override;
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    void bind_compute_shader(const WeakPointer<ComputeShader> &shader) override;
    void bind_descriptor_sets(const WeakPointer<ComputeShader> &shader, const WeakPointer<DescriptorSet> &descriptorSet, uint32_t setIndex) override;

    void cleanup();
    void recreate(uint32_t imageCount);

    inline const std::vector<VkCommandBuffer>& native_command_buffers() { return m_commandBuffers; }

private:
    VulkanCommandBufferCreateInfo m_vkCreateInfo;
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    WeakPointer<VulkanShader> m_boundShader;
    bool m_finished = false;
    bool m_cleared = true;
};

}

#endif //EAGLE_VULKANCOMMANDBUFFER_H
