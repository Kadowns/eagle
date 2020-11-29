#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/core/renderer/DescriptorSet.h"
#include "VulkanCore.h"
#include "VulkanShader.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture.h"
#include "VulkanCleaner.h"

EG_BEGIN

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};


class VulkanDescriptorSet : public DescriptorSet, public VulkanCleanable {
public:
    VulkanDescriptorSet(const Reference<VulkanDescriptorSetLayout> &descriptorSetLayout,
                        const std::vector<Reference<DescriptorItem>> &descriptorItems,
                        const VulkanDescriptorSetCreateInfo& createInfo);

    VulkanDescriptorSet(const Reference<VulkanDescriptorSetLayout>& descriptorSetLayout, const VulkanDescriptorSetCreateInfo& createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void update_descriptor_sets();
    void create_descriptor_sets();

    virtual void update(const std::vector<Reference<DescriptorItem>> &descriptorItems) override;
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}

    virtual bool is_dirty() const override;

    virtual void flush(uint32_t index) override;

private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::set<int> m_dirtyDescriptors;
    Handle<VulkanDescriptorSetLayout> m_descriptorSetLayout;
    std::vector<Reference<DescriptorItem>> m_descriptorItems;
    VulkanDescriptorSetCreateInfo m_info;

    bool m_cleared = true;

};

EG_END

#endif //EAGLE_VULKANDESCRIPTORSET_H
