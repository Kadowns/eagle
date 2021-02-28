//
// Created by Novak on 04/08/2019.
//

#ifndef EAGLE_VULKANDESCRIPTORSETLAYOUT_H
#define EAGLE_VULKANDESCRIPTORSETLAYOUT_H

#include "VulkanCore.h"
#include "eagle/renderer/DescriptorSetLayout.h"

namespace eagle {

class VulkanDescriptorSetLayout : public DescriptorSetLayout{
public:
    VulkanDescriptorSetLayout(VkDevice device, const std::vector<DescriptorBindingDescription>& bindings);
    virtual ~VulkanDescriptorSetLayout();

    void create_layout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    virtual const std::vector<DescriptorBindingDescription>& bindings();

    inline VkDescriptorSetLayout& get_native_layout() { return m_layout; }
    inline const std::vector<VkDescriptorSetLayoutBinding>& get_native_bindings() { return m_nativeBindings; }

private:
    VkDevice m_device;
    VkDescriptorSetLayout m_layout;
    std::vector<DescriptorBindingDescription> m_bindings;
    std::vector<VkDescriptorSetLayoutBinding> m_nativeBindings;


};

}

#endif //EAGLE_VULKANDESCRIPTORSETLAYOUT_H
