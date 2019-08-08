//
// Created by Novak on 04/08/2019.
//

#ifndef EAGLE_VULKANDESCRIPTORSETLAYOUT_H
#define EAGLE_VULKANDESCRIPTORSETLAYOUT_H

#include "VulkanCore.h"
#include "eagle/renderer/DescriptorSetLayout.h"

_EAGLE_BEGIN

class VulkanDescriptorSetLayout : public DescriptorSetLayout{
public:
    VulkanDescriptorSetLayout(VkDevice device, const std::vector<DescriptorBinding>& bindings);
    virtual ~VulkanDescriptorSetLayout();

    virtual void create(const std::vector<DescriptorBinding> &bindings) override;

    inline VkDescriptorSetLayout& get_native_layout() { return m_layout; }
    inline const std::vector<VkDescriptorSetLayoutBinding>& get_native_bindings() { return m_bindings; }

private:
    VkDevice m_device;
    VkDescriptorSetLayout m_layout;
    std::vector<VkDescriptorSetLayoutBinding> m_bindings;


};

_EAGLE_END

#endif //EAGLE_VULKANDESCRIPTORSETLAYOUT_H
