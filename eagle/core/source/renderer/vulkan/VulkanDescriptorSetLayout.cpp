//
// Created by Novak on 04/08/2019.
//

#include <eagle/core/renderer/vulkan/VulkanDescriptorSetLayout.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>

EG_BEGIN

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const std::vector<DescriptorBindingDescription> &bindings) :
    m_device(device), m_bindings(bindings) {
    std::vector<VkDescriptorSetLayoutBinding> vkBindings(bindings.size());
    for (uint32_t i = 0; i < bindings.size(); i++){
        VkDescriptorSetLayoutBinding descriptorBinding = {};

        descriptorBinding.binding = bindings[i].binding;
        descriptorBinding.descriptorCount = 1;
        descriptorBinding.pImmutableSamplers = nullptr;
        descriptorBinding.descriptorType = VulkanConverter::to_vk(bindings[i].descriptorType);
        descriptorBinding.stageFlags = VulkanConverter::to_vk(bindings[i].shaderStage);
        vkBindings[i] = descriptorBinding;
    }

    create_layout(vkBindings);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
    VK_CALL vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
}

void VulkanDescriptorSetLayout::create_layout(const std::vector<VkDescriptorSetLayoutBinding> &bindings) {

    m_nativeBindings = bindings;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pBindings = bindings.data();
    createInfo.bindingCount = static_cast<uint32_t>(bindings.size());

    VK_CALL vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &m_layout);
}

const std::vector<DescriptorBindingDescription> &VulkanDescriptorSetLayout::bindings() {
    return m_bindings;
}

EG_END