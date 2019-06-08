//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXLAYOUT_H
#define EAGLE_VULKANVERTEXLAYOUT_H

#include <vector>
#include "VulkanCore.h"

_EAGLE_BEGIN

enum VERTEX_COMPONENT {
    VERTEX_COMPONENT_FLOAT = 0x1,
    VERTEX_COMPONENT_VEC2 = 0x2,
    VERTEX_COMPONENT_VEC3 = 0x8,
    VERTEX_COMPONENT_VEC4 = 0x16
};

class VulkanVertexLayout {

public:
    VulkanVertexLayout(std::vector<VERTEX_COMPONENT> components);
    ~VulkanVertexLayout() = default;

    size_t stride();

    VkVertexInputBindingDescription get_binding_description();
    std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

private:

    VkFormat component_format(const VERTEX_COMPONENT& component);
    uint32_t size(const VERTEX_COMPONENT& component);
    std::vector<VERTEX_COMPONENT> m_components;

};

_EAGLE_END

#endif //EAGLE_VULKANVERTEXLAYOUT_H
