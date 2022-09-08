//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include <eagle/renderer/command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_queue.h>

#include <thread>

namespace eagle {

class VulkanSharedCommandPool;

struct VulkanCommandBufferCreateInfo {
    VkDevice device = VK_NULL_HANDLE;
    uint32_t frameCount = 0;
    VulkanQueue* queue = nullptr;
    uint32_t* currentFrame = nullptr;
};

class VulkanCommandBuffer : public CommandBuffer {
private:

    struct ThreadCommandBuffer {
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        std::thread::id threadId;
    };

public:

    VulkanCommandBuffer(const CommandBufferCreateInfo& createInfo, const VulkanCommandBufferCreateInfo& vkCreateInfo);
    ~VulkanCommandBuffer() override;

    void begin() override;
    void begin(RenderPass* renderPass, Framebuffer* framebuffer) override;
    void end() override;
    void execute_commands(const std::span<CommandBuffer*>& commandBuffers) override;

    void begin_render_pass(RenderPass* renderPass, Framebuffer* framebuffer) override;
    void end_render_pass() override;
    void bind_shader(Shader* shader) override;
    void bind_vertex_buffer(VertexBuffer* vertexBuffer, uint32_t binding) override;
    void bind_index_buffer(IndexBuffer* indexBuffer) override;
    void push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    void bind_descriptor_sets(DescriptorSet* descriptorSet, uint32_t setIndex) override;
    void draw(uint32_t vertexCount) override;
    void draw_indexed(uint32_t indicesCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override;
    void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;
    void pipeline_barrier(Image* image, PipelineStageFlags srcPipelineStages, PipelineStageFlags dstPipelineStages) override;
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    VkCommandBuffer native_command_buffer(uint32_t index);

private:

    ThreadCommandBuffer& prepare_command_buffer(uint32_t frameIndex);

private:

    VulkanCommandBufferCreateInfo m_vkCreateInfo;
    std::vector<ThreadCommandBuffer> m_threadCommandBuffers;
    uint32_t m_currentFrame = 0;
    VkCommandBuffer m_currentCommandBuffer = VK_NULL_HANDLE;
    VulkanShader* m_boundShader = nullptr;
};

}

#endif //EAGLE_VULKANCOMMANDBUFFER_H
