#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include <eagle/renderer/descriptor_set.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_shader.h>
#include <eagle/renderer/vulkan/vulkan_uniform_buffer.h>
#include <eagle/renderer/vulkan/vulkan_texture.h>
#include <eagle/renderer/vulkan/vulkan_cleaner.h>

namespace eagle {

struct VulkanDescriptorSetInfo {
    VkDevice device;
    uint32_t frameCount;
};

class VulkanDescriptorSet : public DescriptorSet, public VulkanCleanable {
public:
    VulkanDescriptorSet(DescriptorSetCreateInfo info, VulkanDescriptorSetInfo vkInfo);
    ~VulkanDescriptorSet() override;

    void cleanup();
    void recreate(uint32_t bufferCount);

    void create_descriptor_sets();

    void update() override;
    VkDescriptorSet native_descriptor_set(uint32_t frameIndex);

    bool is_dirty() const override;
    bool valid() const;

    void flush(uint32_t index) override;
    void flush_all();

private:
    VulkanDescriptorSetInfo m_vkInfo;
    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::set<uint32_t> m_dirtyDescriptors;
    bool m_cleared = true;

};

}

#endif //EAGLE_VULKANDESCRIPTORSET_H
