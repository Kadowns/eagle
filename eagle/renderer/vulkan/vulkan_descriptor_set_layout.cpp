//
// Created by Novak on 04/08/2019.
//

#include <eagle/renderer/vulkan/vulkan_descriptor_set_layout.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>

namespace eagle {




VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
        DescriptorSetLayoutInfo info,
        VulkanDescriptorSetLayoutInfo vkInfo) :
        DescriptorSetLayout(std::move(info)),
        m_vkInfo(vkInfo){
    std::vector<VkDescriptorSetLayoutBinding> vkBindings(m_info.bindings.size());
    for (uint32_t i = 0; i < m_info.bindings.size(); i++){
        VkDescriptorSetLayoutBinding descriptorBinding = {};

        descriptorBinding.binding = m_info.bindings[i].binding;
        descriptorBinding.descriptorCount = 1;
        descriptorBinding.pImmutableSamplers = nullptr;
        descriptorBinding.descriptorType = VulkanConverter::to_vk(m_info.bindings[i].descriptorType);
        descriptorBinding.stageFlags = VulkanConverter::to_vk(m_info.bindings[i].shaderStage);
        vkBindings[i] = descriptorBinding;
    }
    create_layout(vkBindings);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(m_vkInfo.device, m_layout, nullptr);
}

void VulkanDescriptorSetLayout::create_layout(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {

    m_nativeBindings = bindings;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pBindings = bindings.data();
    createInfo.bindingCount = static_cast<uint32_t>(bindings.size());

    vkCreateDescriptorSetLayout(m_vkInfo.device, &createInfo, nullptr, &m_layout);
}

}