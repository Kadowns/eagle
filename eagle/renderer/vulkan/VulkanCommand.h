//
// Created by Novak on 24/06/2019.
//

#ifndef EAGLE_VULKANCOMMAND_H
#define EAGLE_VULKANCOMMAND_H

#include "VulkanCore.h"

#include <array>

_EAGLE_BEGIN

class VulkanCommand {
public:
    inline virtual void operator()() = 0;
};


class VulkanCommandDrawIndexed : public VulkanCommand {

public:

    explicit VulkanCommandDrawIndexed(VkCommandBuffer &commandBuffer, uint32_t indicesCount, uint32_t indexOffset,
                                      uint32_t vertexOffset) :
        m_commandBuffer(commandBuffer), m_indicesCount(indicesCount), m_vertexOffset(vertexOffset), m_indexOffset(indexOffset){}

    inline virtual void operator()() override {
        vkCmdDrawIndexed(m_commandBuffer, m_indicesCount, 1, m_indexOffset, m_vertexOffset, 0);
    }

private:

    VkCommandBuffer m_commandBuffer;
    uint32_t m_indicesCount;
    uint32_t m_vertexOffset, m_indexOffset;
};

class VulkanCommandBindVertexBuffer : public VulkanCommand {

public:

    explicit VulkanCommandBindVertexBuffer(VkCommandBuffer& commandBuffer, VkBuffer& vertexBuffer) :
        m_commandBuffer(commandBuffer), m_vertexBuffer(vertexBuffer){}

    inline virtual void operator()() override {
        //VkBuffer buffers[] = {m_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, &m_vertexBuffer, offsets);
    }
private:
    VkCommandBuffer m_commandBuffer;
    VkBuffer m_vertexBuffer;
};

class VulkanCommandDraw : public VulkanCommand {

public:
    VulkanCommandDraw(VkCommandBuffer& commandBuffer, uint32_t verticesCount) :
        m_commandBuffer(commandBuffer), m_verticesCount(verticesCount){}

    inline virtual void operator()() override {
        vkCmdDraw(m_commandBuffer, m_verticesCount, 1, 0, 0);
    }

private:

    VkCommandBuffer m_commandBuffer;
    uint32_t m_verticesCount;


};

class VulkanCommandBindIndexBuffer : public VulkanCommand {

public:
    explicit VulkanCommandBindIndexBuffer(VkCommandBuffer& commandBuffer, VkBuffer& indexBuffer, VkIndexType indexType) :
        m_commandBuffer(commandBuffer), m_indexBuffer(indexBuffer), m_indexType(indexType){}

    inline virtual void operator()() override {
        vkCmdBindIndexBuffer(m_commandBuffer, m_indexBuffer, 0, m_indexType);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkBuffer m_indexBuffer;
    VkIndexType m_indexType;
};

class VulkanCommandBindShader : public VulkanCommand {

public:
    explicit VulkanCommandBindShader(VkCommandBuffer& commandBuffer, VkPipeline& pipeline) :
    m_commandBuffer(commandBuffer), m_pipeline(pipeline){}

    inline virtual void operator()() override {
        vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkPipeline m_pipeline;

};

class VulkanCommandBindDescriptorSet : public VulkanCommand {

public:
    explicit VulkanCommandBindDescriptorSet(VkCommandBuffer &commandBuffer, VkPipelineLayout &pipelineLayout,
                                            VkDescriptorSet &descriptorSet, uint32_t setIndex) :
    m_commandBuffer(commandBuffer), m_pipelineLayout(pipelineLayout), m_descriptorSet(descriptorSet), m_setIndex(setIndex){}

    inline virtual void operator()() override {
        vkCmdBindDescriptorSets(
                m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout,
                m_setIndex, 1, &m_descriptorSet,
                0, nullptr);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSet m_descriptorSet;
    uint32_t m_setIndex;
};

class VulkanCommandPushConstants : public VulkanCommand {

public:
    explicit VulkanCommandPushConstants(VkCommandBuffer &commandBuffer, VkPipelineLayout &layout,
                                        VkShaderStageFlags stageFlags,
                                        uint32_t offset, size_t size, void *data) :
            m_commandBuffer(commandBuffer),
            m_pipelineLayout(layout),
            m_stageFlags(stageFlags),
            m_offset(offset),
            m_size(size),
            m_data(data){}

    inline virtual void operator()() override {
        vkCmdPushConstants(m_commandBuffer, m_pipelineLayout, m_stageFlags, m_offset, m_size, m_data);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkPipelineLayout m_pipelineLayout;
    VkShaderStageFlags m_stageFlags;
    uint32_t m_offset;
    size_t m_size;
    void* m_data;
};

class VulkanCommandSetViewport : public VulkanCommand {

public:
    explicit VulkanCommandSetViewport(VkCommandBuffer &commandBuffer, VkViewport viewport):
    m_commandBuffer(commandBuffer), m_viewport(viewport){}

    inline virtual void operator()()override {

        vkCmdSetViewport(m_commandBuffer, 0, 1, &m_viewport);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkViewport m_viewport;
};

class VulkanCommandSetScissor: public VulkanCommand {

public:
    explicit VulkanCommandSetScissor(VkCommandBuffer &commandBuffer, VkRect2D rect2D):
            m_commandBuffer(commandBuffer), m_rect2D(rect2D){}

    inline virtual void operator()()override {
        vkCmdSetScissor(m_commandBuffer, 0, 1, &m_rect2D);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkRect2D m_rect2D;
};


class VulkanCommandBeginRenderPass : public VulkanCommand {

public:
    explicit VulkanCommandBeginRenderPass(VkCommandBuffer &commandBuffer, VkRenderPass &renderPass,
                                          VkFramebuffer &frameBuffer,
                                          VkExtent2D &extent, std::vector<VkClearValue> clearValues) :
        m_commandBuffer(commandBuffer),
        m_renderPass(renderPass),
        m_framebuffer(frameBuffer),
        m_extent(extent),
        m_clearValues(std::move(clearValues)) {}

    inline virtual void operator()() override {

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_extent;
        renderPassInfo.clearValueCount = m_clearValues.size();
        renderPassInfo.pClearValues = m_clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkRenderPass m_renderPass;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;
    std::vector<VkClearValue> m_clearValues;
};

class VulkanCommandEndRenderPass : public VulkanCommand{

public:
    explicit VulkanCommandEndRenderPass(VkCommandBuffer& commandBuffer) : m_commandBuffer(commandBuffer) {}

    inline virtual void operator()() override{
        vkCmdEndRenderPass(m_commandBuffer);
    }
private:
    VkCommandBuffer m_commandBuffer;
};

_EAGLE_END

#endif //EAGLE_VULKANCOMMAND_H
