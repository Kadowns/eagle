#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/core/renderer/DescriptorSet.h"
#include "VulkanCore.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture2D.h"

EG_BEGIN

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};


class VulkanDescriptorSet : public DescriptorSet {
public:
    VulkanDescriptorSet(const Reference<VulkanDescriptorSetLayout> &descriptorSetLayout,
                        const std::vector<Reference<DescriptorItem>> &descriptorItems,
                        VulkanDescriptorSetCreateInfo createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void create_descriptor_sets();
    void create_descriptor_pool();
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}
    
private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    Handle<VulkanDescriptorSetLayout> m_descriptorSetLayout;
    std::vector<Reference<DescriptorItem>> m_descriptorItems;
    VulkanDescriptorSetCreateInfo m_info;

    bool m_cleared = true;

};

EG_END

#endif //EAGLE_VULKANDESCRIPTORSET_H
