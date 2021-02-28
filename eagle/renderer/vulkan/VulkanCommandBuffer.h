//
// Created by Novak on 27/09/2019.
//

#ifndef EAGLE_VULKANCOMMANDBUFFER_H
#define EAGLE_VULKANCOMMANDBUFFER_H

#include "VulkanCore.h"
#include "VulkanShader.h"

#include <eagle/renderer/CommandBuffer.h>

namespace eagle {

class VulkanCommandBuffer : public CommandBuffer {
public:

    VulkanCommandBuffer(VkDevice &device, VkCommandPool &commandPool, uint32_t &imageIndexRef);
    virtual ~VulkanCommandBuffer();


    virtual void begin() override;
    virtual void finish() override;
    virtual bool is_finished() override;
    virtual void begin_render_pass(const std::shared_ptr<RenderPass> &renderPass, const std::shared_ptr<Framebuffer>& framebuffer) override;
    virtual void end_render_pass() override;
    virtual void bind_shader(const std::shared_ptr<Shader> &shader) override;
    virtual void bind_vertex_buffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;
    virtual void bind_index_buffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;
    virtual void push_constants(ShaderStage stage, uint32_t offset, size_t size, void *data) override;
    virtual void bind_descriptor_sets(const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) override;
    virtual void draw(uint32_t vertexCount) override;
    virtual void draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) override;
    virtual void set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;
    virtual void set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;
    virtual void pipeline_barrier(const std::shared_ptr<Image> &image, const std::vector<PipelineStage> &srcPipelineStages, const std::vector<PipelineStage> &dstPipelineStages) override;
    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    virtual void bind_compute_shader(const std::shared_ptr<ComputeShader> &shader) override;
    virtual void bind_descriptor_sets(const std::shared_ptr<ComputeShader> &shader, const std::shared_ptr<DescriptorSet> &descriptorSet, uint32_t setIndex) override;

    inline VkCommandBuffer& native_command_buffer() { return m_commandBuffer; }

private:
    VkDevice& m_device;
    VkCommandPool& m_commandPool;
    uint32_t& m_imageIndexRef;
    VkCommandBuffer m_commandBuffer;
    bool m_finished = false;
    std::shared_ptr<VulkanShader> m_boundShader;
};

}

#endif //EAGLE_VULKANCOMMANDBUFFER_H
