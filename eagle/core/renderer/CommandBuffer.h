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

EG_BEGIN

class CommandBuffer {
public:

    virtual ~CommandBuffer() = default;

    virtual void begin() = 0;

    virtual void finish() = 0;

    virtual bool is_finished() = 0;

    virtual void
    bind_shader(const Reference<Shader> &shader) = 0;

    virtual void
    bind_compute_shader(const Reference<ComputeShader>& shader) = 0;

    virtual void
    draw(uint32_t vertexCount) = 0;

    virtual void
    bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) = 0;

    virtual void
    bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) = 0;

    virtual void
    push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) = 0;

    virtual void
    draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

    virtual void
    bind_descriptor_sets(const Reference<DescriptorSet> &descriptorSet, uint32_t setIndex) = 0;

    virtual void
    bind_descriptor_sets(const Reference<ComputeShader> &shader, const Reference<DescriptorSet> &descriptorSet,
                                      uint32_t setIndex) = 0;

    virtual void
    set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) = 0;

    virtual void
    set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) = 0;

    virtual void
    end_render_pass() = 0;


    virtual void
    begin_render_pass(const Reference<RenderPass> &renderPass, const Reference<Framebuffer>& framebuffer) = 0;

    virtual void
    pipeline_barrier(const Reference <Image> &image, ShaderStage srcStage,
                     ShaderStage dstStage) = 0;

    virtual void
    dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    virtual void submit() = 0;

};

EG_END

#endif //EAGLE_COMMANDBUFFER_H
