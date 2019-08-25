//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/Core.h"
#include "RenderingCore.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Texture2D.h"
#include "RenderTarget.h"

EG_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() {}
    virtual ~RenderingContext() = default;

    virtual void init(Window *window) = 0;
    virtual bool begin_draw_commands() = 0;

    virtual void end_draw_commands() = 0;
    virtual void refresh() = 0;
    virtual void deinit() = 0;
    virtual void handle_window_resized(int width, int height) = 0;

    virtual Handle<Shader>
    create_shader(const std::string &vertFilePath, const std::string &fragFilePath,
                  const std::vector<Reference<DescriptorSetLayout>> &descriptorSetLayouts,
                  const ShaderPipelineInfo &pipelineInfo) = 0;

//    virtual Handle<Shader>
//    create_shader(const std::string &vertFilePath, const std::string &fragFilePath,
//                  const std::vector<Reference<DescriptorSetLayout>> &descriptorSetLayouts,
//                  const ShaderPipelineInfo &pipelineInfo,
//                  Reference<RenderTarget> renderTarget) = 0;

    virtual Handle<VertexBuffer>
    create_vertex_buffer(void *vertices, uint32_t count, const VertexLayout &vertexLayout,
                         EG_BUFFER_USAGE usageFlags) = 0;

    virtual Handle<IndexBuffer>
    create_index_buffer(void *indexData, size_t indexCount, INDEX_BUFFER_TYPE indexType, EG_BUFFER_USAGE usage) = 0;

    virtual Handle<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) = 0;

    virtual Handle<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBinding> &bindings) = 0;

    virtual Handle<DescriptorSet>
    create_descriptor_set(const Reference<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<Reference<DescriptorItem>> &descriptorItems) = 0;

    virtual Handle<Texture2D>
    create_texture_2d(Texture2DCreateInfo &createInfo) = 0;

    virtual Handle<RenderTarget>
    create_render_target(const std::vector<RENDER_TARGET_ATTACHMENT> &attachments) = 0;

    virtual void
    bind_shader(const Reference<Shader> &shader) = 0;

    virtual void
    draw(uint32_t vertexCount) = 0;

    virtual void
    bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) = 0;

    virtual void
    bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) = 0;

    virtual void
    push_constants(const Reference<Shader> &shader, EG_SHADER_STAGE stage, uint32_t offset, size_t size,
                   void *data) = 0;

    virtual void
    draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

    virtual void
    uniform_buffer_flush(const Reference<UniformBuffer> &uniformBuffer, void *data) = 0;

    virtual void
    vertex_buffer_flush(const Reference<VertexBuffer> &vertexBuffer, void *data, uint32_t vertexCount) = 0;

    virtual void
    index_buffer_flush(const Reference<IndexBuffer> &indexBuffer, void *data, uint32_t indexCount) = 0;

    virtual void
    bind_descriptor_sets(const Reference<Shader> &shader, const Reference<DescriptorSet> &descriptorSet,
                         uint32_t setIndex) = 0;

    virtual void
    set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) = 0;

    virtual void
    set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) = 0;

    virtual void
    end_draw_offscreen() = 0;

    virtual void
    begin_draw_offscreen(const Reference<RenderTarget> &renderTarget) = 0;

};

EG_END

#endif //EAGLE_RENDERINGCONTEXT_H
