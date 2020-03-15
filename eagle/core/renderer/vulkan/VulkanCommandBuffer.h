//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include "VulkanCore.h"

#include <eagle/core/renderer/CommandBuffer.h>

EG_BEGIN

class VulkanCommandBuffer : public CommandBuffer {
public:

    VulkanCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);
    virtual ~VulkanCommandBuffer();


    virtual void finish() override;
    virtual bool is_finished() override;
    virtual void begin_render_pass(const Reference<RenderTarget> &renderTarget) override;
    virtual void end_render_pass() override;
    virtual void bind_shader(const Reference<Shader> &shader) override;
    virtual void bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) override;
    virtual void bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) override;
    virtual void push_constants(const Reference<Shader> &shader, ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    virtual void bind_descriptor_sets(const Reference<Shader> &shader, const Reference<DescriptorSet> &descriptorSet, uint32_t setIndex) override;
    virtual void draw(uint32_t vertexCount) override;
    virtual void draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) override;
    virtual void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    virtual void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;

    inline VkCommandBuffer& native_command_buffer() { return m_commandBuffer; }

private:
    VkCommandBuffer& m_commandBuffer;
    uint32_t m_imageIndex;
    bool m_finished;
};

EG_END

#endif //EAGLE_VULKANCOMMANDBUFFER_H
