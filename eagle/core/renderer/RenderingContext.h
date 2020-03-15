//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/GlobalDefinitions.h"
#include "RenderingCore.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Texture2D.h"
#include "RenderTarget.h"
#include "CommandBuffer.h"

EG_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() {}
    virtual ~RenderingContext() = default;

    virtual void init(Window *window) = 0;
    virtual void deinit() = 0;
    virtual void handle_window_resized(int width, int height) = 0;

    virtual bool prepare_frame() = 0;
    virtual Scope <Eagle::CommandBuffer> create_command_buffer() = 0;
    virtual const Reference <RenderTarget> main_render_target() = 0;
    virtual void submit_command_buffer(Scope <Eagle::CommandBuffer> &commandBuffer) = 0;
    virtual void present_frame() = 0;

    virtual Handle <Shader>
    create_shader(const std::unordered_map<ShaderStage, std::string> &shaderPaths,
                  const ShaderPipelineInfo &pipelineInfo) = 0;

    virtual Handle<VertexBuffer>
    create_vertex_buffer(void *vertices, uint32_t count, const VertexLayout &vertexLayout,
                         BufferUsage usageFlags) = 0;

    virtual Handle<IndexBuffer>
    create_index_buffer(void *indexData, size_t indexCount, IndexBufferType indexType, BufferUsage usage) = 0;

    virtual Handle<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) = 0;

    virtual Handle<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBindingDescription> &bindings) = 0;

    virtual Handle<DescriptorSet>
    create_descriptor_set(const Reference<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<Reference<DescriptorItem>> &descriptorItems) = 0;

    virtual Handle<Texture2D>
    create_texture_2d(const Texture2DCreateInfo &createInfo) = 0;

    virtual Handle<RenderTarget>
    create_render_target(const std::vector<RENDER_TARGET_ATTACHMENT> &attachments) = 0;

};

EG_END

#endif //EAGLE_RENDERINGCONTEXT_H
