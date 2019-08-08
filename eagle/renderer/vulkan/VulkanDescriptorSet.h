#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/renderer/DescriptorSet.h"
#include "VulkanCore.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture2D.h"

_EAGLE_BEGIN

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};


class VulkanDescriptorSet : public DescriptorSet {
public:
    VulkanDescriptorSet(const std::shared_ptr<VulkanDescriptorSetLayout> &descriptorSetLayout,
                        const std::vector<std::shared_ptr<DescriptorItem>> &descriptorItems,
                        VulkanDescriptorSetCreateInfo createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void create_descriptor_sets();
    void create_descriptor_pool();
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}
    
private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::weak_ptr<VulkanDescriptorSetLayout> m_descriptorSetLayout;
    std::vector<std::shared_ptr<DescriptorItem>> m_descriptorItems;
    VulkanDescriptorSetCreateInfo m_info;

    bool m_cleared = true;

};

_EAGLE_END

#endif //EAGLE_VULKANDESCRIPTORSET_H
