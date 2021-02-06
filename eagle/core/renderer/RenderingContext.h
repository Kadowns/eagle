//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/CoreGlobalDefinitions.h"
#include "eagle/core/events/EventBus.h"
#include "RenderingCore.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "CommandBuffer.h"
#include "ComputeShader.h"
#include "StorageBuffer.h"
#include "Framebuffer.h"
#include "RenderPass.h"

EG_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() = default;
    virtual ~RenderingContext() = default;

    virtual void init(Window *window, EventBus* eventBus) = 0;
    virtual void deinit() = 0;

    virtual bool prepare_frame() = 0;
    virtual void present_frame() = 0;
    virtual void submit_command_buffer(const Reference<CommandBuffer>& commandBuffer) = 0;

    virtual Reference<CommandBuffer> main_command_buffer() = 0;
    virtual Reference<RenderPass> main_render_pass() = 0;
    virtual Reference<Framebuffer> main_frambuffer() = 0;

    virtual Handle <Shader>
    create_shader(const ShaderCreateInfo &shaderCreateInfo) = 0;

    virtual Handle<VertexBuffer>
    create_vertex_buffer(void *vertices, uint32_t size, const VertexLayout &vertexLayout,
                         UpdateType usageFlags) = 0;

    virtual Handle<IndexBuffer>
    create_index_buffer(void *indexData, size_t indexCount, IndexBufferType indexType, UpdateType usage) = 0;

    virtual Handle<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) = 0;

    virtual Handle <StorageBuffer>
    create_storage_buffer(size_t size, void *data, UpdateType usage) = 0;

    virtual Handle<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBindingDescription> &bindings) = 0;

    virtual Handle<DescriptorSet>
    create_descriptor_set(const Reference<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<Reference<DescriptorItem>> &descriptorItems) = 0;

    virtual Handle<Texture>
    create_texture(const TextureCreateInfo &createInfo) = 0;

    virtual Handle<RenderPass>
    create_render_pass(const std::vector<RenderAttachmentDescription>& colorAttachments, const RenderAttachmentDescription& depthAttachment) = 0;

    virtual Handle<Framebuffer>
    create_framebuffer(const FramebufferCreateInfo& createInfo) = 0;

    virtual Handle<Image>
    create_image(const ImageCreateInfo& createInfo) = 0;

    virtual Handle<ComputeShader>
    create_compute_shader(const std::string& path) = 0;

    virtual void
    destroy_texture_2d(const Reference<Texture>& texture) = 0;

};

struct OnRenderingContextRecreated {
    RenderingContext* context;
};

EG_END

#endif //EAGLE_RENDERINGCONTEXT_H
