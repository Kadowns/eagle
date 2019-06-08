//
// Created by Novak on 06/06/2019.
//

#include "eagle/renderer/vulkan/VulkanVertexLayout.h"

_EAGLE_BEGIN

VulkanVertexLayout::VulkanVertexLayout(std::vector<VERTEX_COMPONENT> components) :
    m_components(std::move(components)){}

size_t VulkanVertexLayout::stride() {
    size_t stride = 0;
    for (auto comp : m_components) {
        stride += size(comp);
    }
    return stride;
}

VkVertexInputBindingDescription VulkanVertexLayout::get_binding_description() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = stride();
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VulkanVertexLayout::get_attribute_descriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    attributeDescriptions.resize(m_components.size());
    uint32_t offset = 0;
    for (uint8_t i = 0; i < m_components.size(); i++) {
        attributeDescriptions[i].binding = 0;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = component_format(m_components[i]);
        attributeDescriptions[i].offset = offset;
        offset += size(m_components[i]);
    }

    return attributeDescriptions;
}

VkFormat VulkanVertexLayout::component_format(const VERTEX_COMPONENT &component) {
    switch (component) {
        case VERTEX_COMPONENT_FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case VERTEX_COMPONENT_VEC2:
            return VK_FORMAT_R32G32_SFLOAT;
        case VERTEX_COMPONENT_VEC3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        default:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
}

uint32_t VulkanVertexLayout::size(const VERTEX_COMPONENT &component) {
    switch (component) {
        case VERTEX_COMPONENT_FLOAT:
            return sizeof(float);
        case VERTEX_COMPONENT_VEC2:
            return sizeof(float) * 2;
        case VERTEX_COMPONENT_VEC3:
            return sizeof(float) * 3;
        default:
            return sizeof(float) * 4;
    }
}

_EAGLE_END


