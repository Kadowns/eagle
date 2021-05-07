#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/renderer/descriptor_set.h"
#include "vulkan_global_definitions.h"
#include "vulkan_shader.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_texture.h"
#include "vulkan_cleaner.h"

namespace eagle {

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};

class VulkanDescriptorSet : public DescriptorSet, public VulkanCleanable {
public:
    VulkanDescriptorSet(const std::weak_ptr<VulkanDescriptorSetLayout> &descriptorSetLayout,
                        const std::vector<std::weak_ptr<DescriptorItem>> &descriptorItems,
                        const VulkanDescriptorSetCreateInfo& createInfo);

    VulkanDescriptorSet(const std::weak_ptr<VulkanDescriptorSetLayout>& descriptorSetLayout, const VulkanDescriptorSetCreateInfo& createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void recreate(uint32_t bufferCount);

    void create_descriptor_sets();

    void update() override;
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}

    virtual bool is_dirty() const override;

    virtual void flush(uint32_t index) override;

private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::set<int> m_dirtyDescriptors;
    std::weak_ptr<VulkanDescriptorSetLayout> m_descriptorSetLayout;
    VulkanDescriptorSetCreateInfo m_info;
    bool m_cleared = true;

};

}

#endif //EAGLE_VULKANDESCRIPTORSET_H
