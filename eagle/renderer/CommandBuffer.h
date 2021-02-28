//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_COMMANDBUFFER_H
#define EAGLE_COMMANDBUFFER_H

#include "RenderingCore.h"
#include "Shader.h"
#include "ComputeShader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorSet.h"
#include "RenderTarget.h"
#include "RenderPass.h"
#include "Framebuffer.h"

namespace eagle {

enum class CommandBufferLevel {
    PRIMARY,
    SECONDARY
};

struct CommandBufferCreateInfo {
    CommandBufferLevel level;
};

class CommandBuffer {
public:

    explicit CommandBuffer(const CommandBufferCreateInfo& createInfo) : m_createInfo(createInfo) {}

    virtual ~CommandBuffer() = default;

    virtual void begin() = 0;

    virtual void finish() = 0;

    virtual bool is_finished() = 0;

    virtual void
    bind_shader(const std::shared_ptr<Shader> &shader) = 0;

    virtual void
    bind_compute_shader(const std::shared_ptr<ComputeShader>& shader) = 0;

    virtual void
    draw(uint32_t vertexCount) = 0;

    virtual void
    bind_vertex_buffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;

    virtual void
    bind_index_buffer(const std::shared_ptr<IndexBuffer> &indexBuffer) = 0;

    virtual void
    push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) = 0;

    virtual void
    draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

    virtual void
    bind_descriptor_sets(const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) = 0;

    virtual void
    bind_descriptor_sets(const std::shared_ptr<ComputeShader> &shader, const std::shared_ptr<DescriptorSet> &descriptorSet,
                                      uint32_t setIndex) = 0;

    virtual void
    set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) = 0;

    virtual void
    set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) = 0;

    virtual void
    end_render_pass() = 0;

    virtual void
    begin_render_pass(const std::shared_ptr<RenderPass> &renderPass, const std::shared_ptr<Framebuffer>& framebuffer) = 0;

    virtual void
    pipeline_barrier(const std::shared_ptr<Image> &image, const std::vector<PipelineStage> &srcPipelineStages,
                     const std::vector<PipelineStage> &dstPipelineStages) = 0;

    virtual void
    dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

protected:
    CommandBufferCreateInfo m_createInfo;

};

}

#endif //EAGLE_COMMANDBUFFER_H
