#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/renderer/DescriptorSet.h"
#include "eagle/renderer/vulkan/VulkanCore.h"
#include "eagle/renderer/vulkan/VulkanShader.h"
#include "eagle/renderer/vulkan/VulkanUniformBuffer.h"

_EAGLE_BEGIN

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};

struct VulkanDescriptorSetDrawInfo {
    VkCommandBuffer commandBuffer;
    uint32_t bufferIndex;
};


class VulkanDescriptorSet : public DescriptorSet {
public:
    VulkanDescriptorSet(std::shared_ptr<VulkanShader> shader,
                           const std::vector<std::shared_ptr<VulkanUniformBuffer>> &uniformBuffers,
                           VulkanDescriptorSetCreateInfo createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void create_descriptor_sets();
    void create_descriptor_pool();
    void set_draw_info(VulkanDescriptorSetDrawInfo drawInfo);
    virtual void bind() override;

private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::vector<std::shared_ptr<VulkanUniformBuffer>> m_uniformBuffers;
    std::weak_ptr<VulkanShader> m_shader;
    VulkanDescriptorSetCreateInfo m_info;
    VulkanDescriptorSetDrawInfo m_drawInfo;

    bool m_cleared = true;

};

_EAGLE_END

#endif //EAGLE_VULKANDESCRIPTORSET_H
