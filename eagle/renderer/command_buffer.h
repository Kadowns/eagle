//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_COMMANDBUFFER_H
#define EAGLE_COMMANDBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/shader.h>
#include <eagle/renderer/compute_shader.h>
#include <eagle/renderer/vertex_buffer.h>
#include <eagle/renderer/index_buffer.h>
#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/descriptor_set.h>
#include <eagle/renderer/render_target.h>
#include <eagle/renderer/render_pass.h>
#include <eagle/renderer/framebuffer.h>
#include <eagle/renderer/command_queue.h>

namespace eagle {

enum class CommandBufferLevel {
    MASTER,
    PRIMARY,
    SECONDARY
};

struct CommandBufferCreateInfo {
    CommandBufferLevel level = CommandBufferLevel::MASTER;
    CommandQueue* queue = nullptr;
};

struct ImageMemoryBarrier {
    AccessFlags srcAccessMask;
    AccessFlags dstAccessMask;
    ImageLayout oldLayout;
    ImageLayout newLayout;
    CommandQueue* srcQueue;
    CommandQueue* dstQueue;
    Image* image;
};

class CommandBuffer {
public:

    explicit CommandBuffer(const CommandBufferCreateInfo& createInfo);

    virtual ~CommandBuffer();

    CommandQueue* command_queue() const;

    virtual void begin() = 0;

    virtual void begin(RenderPass* renderPass, Framebuffer* framebuffer) = 0;

    virtual void end() = 0;

    virtual void execute_commands(const std::span<CommandBuffer*>& commandBuffers) = 0;

    virtual void
    bind_shader(Shader* shader) = 0;

    virtual void
    draw(uint32_t vertexCount) = 0;

    virtual void
    bind_vertex_buffer(VertexBuffer* vertexBuffer, uint32_t binding) = 0;

    virtual void
    bind_index_buffer(IndexBuffer* indexBuffer) = 0;

    virtual void
    push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) = 0;

    virtual void
    draw_indexed(uint32_t indicesCount, uint32_t instanceCount, uint32_t indexOffset,
                 uint32_t vertexOffset, uint32_t instanceOffset) = 0;

    virtual void
    bind_descriptor_sets(DescriptorSet* descriptorSet, uint32_t setIndex) = 0;

    virtual void
    set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) = 0;

    virtual void
    set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) = 0;

    virtual void
    end_render_pass() = 0;

    virtual void
    begin_render_pass(RenderPass* renderPass, Framebuffer* framebuffer) = 0;

    virtual void
    pipeline_barrier(std::span<ImageMemoryBarrier> imageMemoryBarriers, PipelineStageFlags srcPipelineStages,
                     PipelineStageFlags dstPipelineStages) = 0;

    virtual void
    dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

protected:
    CommandBufferCreateInfo m_createInfo;

};

}

#endif //EAGLE_COMMANDBUFFER_H
