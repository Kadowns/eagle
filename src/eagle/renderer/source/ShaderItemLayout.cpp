//
// Created by Novak on 06/06/2019.
//

#include "eagle/renderer/ShaderItemLayout.h"

_EAGLE_BEGIN

ShaderItemLayout::ShaderItemLayout(std::vector<SHADER_ITEM_COMPONENT> components) :
    m_components(std::move(components)){}

size_t ShaderItemLayout::stride() const {
    size_t stride = 0;
    for (auto comp : m_components) {
        stride += offset(comp);
    }
    return stride;
}

uint32_t ShaderItemLayout::offset(const SHADER_ITEM_COMPONENT &component) const {
    switch (component) {
        case SHADER_ITEM_COMPONENT_FLOAT:
            return sizeof(float);
        case SHADER_ITEM_COMPONENT_VEC2:
            return sizeof(float) * 2;
        case SHADER_ITEM_COMPONENT_VEC3:
            return sizeof(float) * 3;
        case SHADER_ITEM_COMPONENT_VEC4:
            return sizeof(float) * 4;
        case SHADER_ITEM_COMPONENT_MAT3:
            return sizeof(float) * 9;
        case SHADER_ITEM_COMPONENT_MAT4:
            return sizeof(float) * 16;
    }
}

size_t ShaderItemLayout::count() const {
    return m_components.size();
}

SHADER_ITEM_COMPONENT ShaderItemLayout::operator[](uint32_t index) const {
    return m_components[index];
}

_EAGLE_END


