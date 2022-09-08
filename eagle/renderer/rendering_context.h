//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include <eagle/core_global_definitions.h>
#include <eagle/events/event.h>
#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/shader.h>
#include <eagle/renderer/vertex_buffer.h>
#include <eagle/renderer/index_buffer.h>
#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/descriptor_set.h>
#include <eagle/renderer/descriptor_set_layout.h>
#include <eagle/renderer/texture.h>
#include <eagle/renderer/command_buffer.h>
#include <eagle/renderer/compute_shader.h>
#include <eagle/renderer/storage_buffer.h>
#include <eagle/renderer/framebuffer.h>
#include <eagle/renderer/render_pass.h>
#include <eagle/renderer/gpu_queue.h>

#include <span>

namespace eagle {

class Window;

struct RenderingContextCreateInfo {
    std::span<GPUQueueCreateInfo> gpuQueueCreateInfos;
};


class RenderingContext {
public:

    struct Properties {
        Format presentFormat;
        Format depthFormat;
    };

public:

    RenderingContext(const RenderingContextCreateInfo& createInfo) : m_createInfo(createInfo) {}
    virtual ~RenderingContext() = default;

    virtual bool prepare_frame(Semaphore* signalAvailableImage) = 0;
    virtual void present_frame() = 0;

    virtual RenderPass* main_render_pass() = 0;
    virtual Framebuffer* main_framebuffer() = 0;

    virtual GPUQueue* find_queue(QueueType type, const std::string& name = "") = 0;

    virtual const Properties& properties() = 0;

    virtual std::shared_ptr<GPUQueue> create_queue(const GPUQueueCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<Shader>
    create_shader(const ShaderCreateInfo &shaderCreateInfo) = 0;

    virtual std::shared_ptr<VertexBuffer>
    create_vertex_buffer(const VertexBufferCreateInfo &createInfo, void *data, size_t size) = 0;

    virtual std::shared_ptr<IndexBuffer>
    create_index_buffer(const IndexBufferCreateInfo &createInfo, void *data, size_t size) = 0;

    virtual std::shared_ptr<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) = 0;

    virtual std::shared_ptr<StorageBuffer>
    create_storage_buffer(size_t size, void *data, UpdateType usage) = 0;

    virtual std::shared_ptr<CommandBuffer>
    create_command_buffer(const CommandBufferCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<DescriptorSetLayout>
    create_descriptor_set_layout(const DescriptorSetLayoutInfo& descriptorSetLayoutInfo) = 0;

    virtual std::shared_ptr<DescriptorSet>
    create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetInfo) = 0;

    virtual std::shared_ptr<Texture>
    create_texture(const TextureCreateInfo &createInfo) = 0;

    virtual std::shared_ptr <RenderPass>
    create_render_pass(RenderPassCreateInfo createInfo) = 0;

    virtual std::shared_ptr<Framebuffer>
    create_framebuffer(const FramebufferCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<Image>
    create_image(const ImageCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<ComputeShader>
    create_compute_shader(const std::string& path) = 0;

    virtual void
    destroy_texture_2d(const std::shared_ptr<Texture>& texture) = 0;

public:
    ImmediateEvent<RenderingContext*> context_recreated;
private:
    RenderingContextCreateInfo m_createInfo;
};

}

#endif //EAGLE_RENDERINGCONTEXT_H
