//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADERITEMLAYOUT_H
#define EAGLE_SHADERITEMLAYOUT_H

#include <vector>
#include "eagle/renderer/vulkan/VulkanCore.h"

_EAGLE_BEGIN

enum SHADER_ITEM_COMPONENT {
    SHADER_ITEM_COMPONENT_FLOAT = BIT(1),
    SHADER_ITEM_COMPONENT_VEC2  = BIT(2),
    SHADER_ITEM_COMPONENT_VEC3  = BIT(3),
    SHADER_ITEM_COMPONENT_VEC4  = BIT(4),
    SHADER_ITEM_COMPONENT_MAT3  = BIT(5),
    SHADER_ITEM_COMPONENT_MAT4  = BIT(6)
};

class ShaderItemLayout {

public:
    ShaderItemLayout(std::vector<SHADER_ITEM_COMPONENT> components);
    ~ShaderItemLayout() = default;

    size_t stride() const;
    uint32_t offset(const SHADER_ITEM_COMPONENT& component) const;
    size_t count() const;

    SHADER_ITEM_COMPONENT operator[] (uint32_t index) const;

private:

    std::vector<SHADER_ITEM_COMPONENT> m_components;

};

_EAGLE_END

#endif //EAGLE_SHADERITEMLAYOUT_H
