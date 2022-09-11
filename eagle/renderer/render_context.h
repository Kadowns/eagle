//
// Created by Ricardo on 22/08/2022.
//

#ifndef EAGLE_RENDERCONTEXT_H
#define EAGLE_RENDERCONTEXT_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/semaphore.h>
#include <eagle/renderer/fence.h>
#include <eagle/renderer/shader.h>
#include <eagle/renderer/descriptor_set.h>
#include <eagle/renderer/image.h>
#include <eagle/renderer/texture.h>
#include <eagle/renderer/vertex_buffer.h>
#include <eagle/renderer/index_buffer.h>
#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/storage_buffer.h>
#include <eagle/renderer/command_buffer.h>
#include <eagle/renderer/command_queue.h>

namespace eagle {

class Window;

struct RenderContextCreateInfo {
    Window* window = nullptr;
    size_t physicalDeviceIndex = 0;
    size_t maxFramesInFlight = 2;
    const char* applicationName = "Unnamed Application";
    const char* engineName = "Unnamed Engine";
};

class RenderContext {
public:

    explicit RenderContext(const RenderContextCreateInfo& createInfo);
    virtual ~RenderContext();

    virtual void init() = 0;
    virtual void exit() = 0;

    virtual bool prepare_frame(Semaphore* signalAvailableImage) = 0;
    virtual void present_frame(std::span<Semaphore*> waitSemaphores) = 0;

    virtual RenderPass* main_render_pass() = 0;
    virtual Framebuffer* main_framebuffer() = 0;
    virtual CommandQueue* command_queue(CommandQueueType queueType) = 0;

    virtual std::shared_ptr<Shader> create_shader(const ShaderCreateInfo &shaderCreateInfo) = 0;

    virtual std::shared_ptr<VertexBuffer> create_vertex_buffer(const VertexBufferCreateInfo &createInfo, void *data, size_t size) = 0;

    virtual std::shared_ptr<IndexBuffer> create_index_buffer(const IndexBufferCreateInfo &createInfo, void *data, size_t size) = 0;

    virtual std::shared_ptr<UniformBuffer> create_uniform_buffer(size_t size, void *data) = 0;

    virtual std::shared_ptr<StorageBuffer> create_storage_buffer(size_t size, void *data, UpdateType usage) = 0;

    virtual std::shared_ptr<CommandBuffer> create_command_buffer(const CommandBufferCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<DescriptorSetLayout> create_descriptor_set_layout(const DescriptorSetLayoutCreateInfo& descriptorSetLayoutInfo) = 0;

    virtual std::shared_ptr<DescriptorSet> create_descriptor_set(const DescriptorSetCreateInfo& descriptorSetInfo) = 0;

    virtual std::shared_ptr<Texture> create_texture(const TextureCreateInfo &createInfo) = 0;

    virtual std::shared_ptr <RenderPass> create_render_pass(RenderPassCreateInfo createInfo) = 0;

    virtual std::shared_ptr<Framebuffer> create_framebuffer(const FramebufferCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<Image> create_image(const ImageCreateInfo& createInfo) = 0;

    virtual std::shared_ptr<Fence> create_fence() = 0;

    virtual std::shared_ptr<Semaphore> create_semaphore() = 0;


protected:
    RenderContextCreateInfo m_createInfo;
};

}

#endif //EAGLE_RENDERCONTEXT_H
