//
// Created by Novak on 24/06/2019.
//

#ifndef EAGLE_VULKANCOMMAND_H
#define EAGLE_VULKANCOMMAND_H

#include "VulkanCore.h"

_EAGLE_BEGIN

class VulkanCommand {
public:
    inline virtual void operator()() = 0;
};


class VulkanCommandDrawIndexed : public VulkanCommand {

public:

    explicit VulkanCommandDrawIndexed(VkCommandBuffer& commandBuffer, uint32_t indicesCount):
        m_commandBuffer(commandBuffer), m_indicesCount(indicesCount){}

    inline virtual void operator()() override {
        vkCmdDrawIndexed(m_commandBuffer, m_indicesCount, 1, 0, 0, 0);
    }

private:

    VkCommandBuffer m_commandBuffer;
    uint32_t m_indicesCount;
};

class VulkanCommandBindVertexBuffer : public VulkanCommand {

public:

    explicit VulkanCommandBindVertexBuffer(VkCommandBuffer& commandBuffer, VkBuffer& vertexBuffer) :
        m_commandBuffer(commandBuffer), m_vertexBuffer(vertexBuffer){}

    inline virtual void operator()() override {
        VkBuffer buffers[] = {m_vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, buffers, offsets);
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
    explicit VulkanCommandBindIndexBuffer(VkCommandBuffer& commandBuffer, VkBuffer& indexBuffer) :
        m_commandBuffer(commandBuffer), m_indexBuffer(indexBuffer){}

    inline virtual void operator()() override {
        vkCmdBindIndexBuffer(m_commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkBuffer m_indexBuffer;
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
    explicit VulkanCommandBindDescriptorSet(VkCommandBuffer& commandBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descriptorSet) :
    m_commandBuffer(commandBuffer), m_pipelineLayout(pipelineLayout), m_descriptorSet(descriptorSet){}

    inline virtual void operator()() override {
        vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkPipelineLayout m_pipelineLayout;
    VkDescriptorSet m_descriptorSet;
};

class VulkanCommandBeginRenderPass: public VulkanCommand{

public:
    explicit VulkanCommandBeginRenderPass(VkCommandBuffer& commandBuffer, VkRenderPass& renderPass, VkFramebuffer& frameBuffer, VkExtent2D& extent):
        m_commandBuffer(commandBuffer), m_renderPass(renderPass), m_framebuffer(frameBuffer), m_extent(extent) {}

    inline virtual void operator()() override {

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_extent;
        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(m_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

private:
    VkCommandBuffer m_commandBuffer;
    VkRenderPass m_renderPass;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;
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
