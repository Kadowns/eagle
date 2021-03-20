//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include "vulkan_global_definitions.h"
#include "vulkan_shader.h"

#include <eagle/renderer/command_buffer.h>

namespace eagle {

struct VulkanCommandBufferCreateInfo {
    VkDevice device;
    VkCommandPool commandPool;
    uint32_t imageCount;
    uint32_t* currentImageIndex = nullptr;
};

class VulkanCommandBuffer : public CommandBuffer {
public:

    VulkanCommandBuffer(const CommandBufferCreateInfo& createInfo, const VulkanCommandBufferCreateInfo& vkCreateInfo);
    virtual ~VulkanCommandBuffer();

    void begin() override;
    void begin(const std::shared_ptr<RenderPass>& renderPass, const std::shared_ptr<Framebuffer>& framebuffer) override;
    void end() override;
    void execute_commands(const std::vector<std::shared_ptr<CommandBuffer>>& commandBuffers) override;

    void begin_render_pass(const std::shared_ptr<RenderPass> &renderPass, const std::shared_ptr<Framebuffer>& framebuffer) override;
    void end_render_pass() override;
    void bind_shader(const std::shared_ptr<Shader> &shader) override;
    void bind_vertex_buffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;
    void bind_index_buffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;
    void push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    void bind_descriptor_sets(const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) override;
    void draw(uint32_t vertexCount) override;
    void draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) override;
    void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;
    void pipeline_barrier(const std::shared_ptr<Image> &image, const std::vector<PipelineStage> &srcPipelineStages, const std::vector<PipelineStage> &dstPipelineStages) override;
    void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;
    void bind_compute_shader(const std::shared_ptr<ComputeShader> &shader) override;
    void bind_descriptor_sets(const std::shared_ptr<ComputeShader> &shader, const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) override;

    void cleanup();
    void recreate(uint32_t imageCount);

    inline const std::vector<VkCommandBuffer>& native_command_buffers() { return m_commandBuffers; }

private:
    VulkanCommandBufferCreateInfo m_vkCreateInfo;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::shared_ptr<VulkanShader> m_boundShader;
    bool m_finished = false;
    bool m_cleared = true;
};

}

#endif //EAGLE_VULKANCOMMANDBUFFER_H
