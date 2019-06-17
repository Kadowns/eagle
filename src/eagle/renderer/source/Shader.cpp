//
// Created by Novak on 06/06/2019.
//
#include "eagle/renderer/Shader.h"

_EAGLE_BEGIN

Shader::Shader() = default;

ShaderItemLayout Shader::get_shader_item(const std::string &name) {
    auto item = m_uniformLayouts.find(name);
    if (item == m_uniformLayouts.end()){
        throw std::runtime_error("invalid uniform name: " + name);
    }
    return (*item).second;
}

_EAGLE_END


