#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/renderer/descriptor_set.h"
#include "vulkan_global_definitions.h"
#include "vulkan_shader.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_texture.h"
#include "vulkan_cleaner.h"


namespace eagle {



struct VulkanDescriptorSetInfo {
    VkDevice device;
    uint32_t bufferCount;
};

class VulkanDescriptorSet : public DescriptorSet, public VulkanCleanable {
public:
    VulkanDescriptorSet(DescriptorSetInfo info, VulkanDescriptorSetInfo vkInfo);
    ~VulkanDescriptorSet() override;

    void cleanup();
    void recreate(uint32_t bufferCount);

    void create_descriptor_sets();

    void update() override;
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}

    bool is_dirty() const override;
    bool valid() const;

    void flush(uint32_t index) override;
    void flush_all();

private:
    VulkanDescriptorSetInfo m_vkInfo;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::set<int> m_dirtyDescriptors;
    bool m_cleared = true;

};

}

#endif //EAGLE_VULKANDESCRIPTORSET_H
