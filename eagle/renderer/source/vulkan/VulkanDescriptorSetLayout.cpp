//
// Created by Novak on 04/08/2019.
//

#include "eagle/renderer/vulkan/VulkanDescriptorSetLayout.h"

_EAGLE_BEGIN

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VkDevice device, const std::vector<DescriptorBinding> &bindings) :
    m_device(device) {
    create(bindings);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
    VK_CALL vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
}

void VulkanDescriptorSetLayout::create(const std::vector<DescriptorBinding> &bindings) {

    for (uint32_t i = 0; i < bindings.size(); i++){
        VkDescriptorSetLayoutBinding descriptorBinding = {};
        
        descriptorBinding.binding = bindings[i].binding;
        descriptorBinding.descriptorCount = 1;
        descriptorBinding.pImmutableSamplers = nullptr;

        VkDescriptorType descriptorType;
        switch(bindings[i].descriptorType){
            case EG_DESCRIPTOR_TYPE::UNIFORM_BUFFER: descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
            case EG_DESCRIPTOR_TYPE::IMAGE_2D: descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; break;
        }
        descriptorBinding.descriptorType = descriptorType;

        VkShaderStageFlags stageFlags;
        switch(bindings[i].shaderStage){
            case EG_SHADER_STAGE::VERTEX: stageFlags = VK_SHADER_STAGE_VERTEX_BIT; break;
            case EG_SHADER_STAGE::FRAGMENT:stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; break;
            case EG_SHADER_STAGE::COMPUTE: stageFlags = VK_SHADER_STAGE_COMPUTE_BIT; break;
            case EG_SHADER_STAGE::GEOMETRY: stageFlags = VK_SHADER_STAGE_GEOMETRY_BIT; break;
            case EG_SHADER_STAGE::TESSALATION_CONTROL: stageFlags = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
            case EG_SHADER_STAGE::TESSALATION_EVALUATE: stageFlags = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
        }
        descriptorBinding.stageFlags = stageFlags;
        m_bindings.push_back(descriptorBinding);
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pBindings = m_bindings.data();
    createInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());

    VK_CALL vkCreateDescriptorSetLayout(m_device, &createInfo, nullptr, &m_layout);
}

_EAGLE_END