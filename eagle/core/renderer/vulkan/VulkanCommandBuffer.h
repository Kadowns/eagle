//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include "VulkanCore.h"
#include "VulkanShader.h"

#include <eagle/core/renderer/CommandBuffer.h>

EG_BEGIN

class VulkanCommandBuffer : public CommandBuffer {
public:

    VulkanCommandBuffer(VkDevice &device, VkCommandPool &commandPool, uint32_t &imageIndexRef,
            const std::function<void(VkCommandBuffer &)> &submit_callback);
    virtual ~VulkanCommandBuffer();


    virtual void begin() override;
    virtual void finish() override;
    virtual bool is_finished() override;
    virtual void begin_render_pass(const Reference<RenderPass> &renderPass, const Reference<Framebuffer>& framebuffer) override;
    virtual void end_render_pass() override;
    virtual void bind_shader(const Reference<Shader> &shader) override;
    virtual void bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) override;
    virtual void bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) override;
    virtual void push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    virtual void bind_descriptor_sets(const Reference <DescriptorSet> &descriptorSet, uint32_t setIndex) override;
    virtual void draw(uint32_t vertexCount) override;
    virtual void draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) override;
    virtual void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    virtual void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;
    virtual void pipeline_barrier(const Reference <Image> &image, ShaderStage srcStage, ShaderStage dstStage) override;
    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    virtual void submit() override;

    virtual void bind_compute_shader(const Reference<ComputeShader> &shader) override;

    virtual void
    bind_descriptor_sets(const Reference<ComputeShader> &shader, const Reference<DescriptorSet> &descriptorSet,
                         uint32_t setIndex) override;

private:
    VkDevice& m_device;
    VkCommandPool& m_commandPool;
    uint32_t& m_imageIndexRef;
    std::function<void(VkCommandBuffer&)> submit_command_buffer_callback;
    VkCommandBuffer m_commandBuffer;
    bool m_finished = false;
    Reference<VulkanShader> m_boundShader;
};

EG_END

#endif //EAGLE_VULKANCOMMANDBUFFER_H
