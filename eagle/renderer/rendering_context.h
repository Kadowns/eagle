//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core_global_definitions.h"
#include "eagle/events/event.h"
#include "renderer_global_definitions.h"
#include "shader.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "uniform_buffer.h"
#include "descriptor_set.h"
#include "descriptor_set_layout.h"
#include "texture.h"
#include "render_target.h"
#include "command_buffer.h"
#include "compute_shader.h"
#include "storage_buffer.h"
#include "framebuffer.h"
#include "render_pass.h"

namespace eagle {

class Window;

class RenderingContext {
public:

    RenderingContext() = default;
    virtual ~RenderingContext() = default;

    virtual bool prepare_frame() = 0;
    virtual void submit_command_buffer(const std::shared_ptr<CommandBuffer>& commandBuffer) = 0;
    virtual void present_frame() = 0;

    virtual std::shared_ptr<RenderPass> main_render_pass() = 0;
    virtual std::shared_ptr<Framebuffer> main_frambuffer() = 0;

    virtual std::weak_ptr<Shader>
    create_shader(const ShaderCreateInfo &shaderCreateInfo) = 0;

    virtual std::weak_ptr<VertexBuffer>
    create_vertex_buffer(const VertexBufferCreateInfo& createInfo) = 0;

    virtual std::weak_ptr<IndexBuffer>
    create_index_buffer(const IndexBufferCreateInfo& createInfo) = 0;

    virtual std::weak_ptr<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) = 0;

    virtual std::weak_ptr<StorageBuffer>
    create_storage_buffer(size_t size, void *data, UpdateType usage) = 0;

    virtual std::weak_ptr<CommandBuffer> create_command_buffer(const CommandBufferCreateInfo& createInfo) = 0;

    virtual std::weak_ptr<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBindingDescription> &bindings) = 0;

    virtual std::weak_ptr<DescriptorSet>
    create_descriptor_set(const std::shared_ptr<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<std::shared_ptr<DescriptorItem>> &descriptorItems) = 0;

    virtual std::weak_ptr<Texture>
    create_texture(const TextureCreateInfo &createInfo) = 0;

    virtual std::weak_ptr<RenderPass>
    create_render_pass(const std::vector<RenderAttachmentDescription>& colorAttachments, const RenderAttachmentDescription& depthAttachment) = 0;

    virtual std::weak_ptr<Framebuffer>
    create_framebuffer(const FramebufferCreateInfo& createInfo) = 0;

    virtual std::weak_ptr<Image>
    create_image(const ImageCreateInfo& createInfo) = 0;

    virtual std::weak_ptr<ComputeShader>
    create_compute_shader(const std::string& path) = 0;

    virtual void
    destroy_texture_2d(const std::shared_ptr<Texture>& texture) = 0;

public:
    ImmediateEvent<RenderingContext*> context_recreated;
};

}

#endif //EAGLE_RENDERINGCONTEXT_H
