//
// Created by Novak on 04/08/2019.
//

#ifndef EAGLE_VULKANDESCRIPTORSETLAYOUT_H
#define EAGLE_VULKANDESCRIPTORSETLAYOUT_H

#include "vulkan_global_definitions.h"
#include "eagle/renderer/descriptor_set_layout.h"

namespace eagle {

struct VulkanDescriptorSetLayoutCreateInfo {
    VkDevice device;
};

class VulkanDescriptorSetLayout : public DescriptorSetLayout {
public:
    VulkanDescriptorSetLayout(DescriptorSetLayoutCreateInfo info, VulkanDescriptorSetLayoutCreateInfo vkInfo);
    virtual ~VulkanDescriptorSetLayout();

    inline VkDescriptorSetLayout& native_layout() { return m_layout; }
    inline const std::vector<VkDescriptorSetLayoutBinding>& native_bindings() { return m_nativeBindings; }

private:
    void create_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

private:
    VulkanDescriptorSetLayoutCreateInfo m_vkInfo;
    VkDescriptorSetLayout m_layout;
    std::vector<VkDescriptorSetLayoutBinding> m_nativeBindings;

};

}

#endif //EAGLE_VULKANDESCRIPTORSETLAYOUT_H
