//
// Created by Novak on 18/07/2019.
//

#include <utility>

#include "vertex_layout.h"

namespace eagle {

size_t VertexInputBindingDescription::stride() const {
    size_t stride = 0;
    for (auto& attribute : attributes){
        stride += format_size(attribute);
    }
    return stride;
}

VertexLayout::VertexLayout() = default;
VertexLayout::~VertexLayout() = default;

size_t VertexLayout::stride() const {
    size_t stride = 0;
    for (auto& binding : m_bindings){
        stride += binding.stride();
    }
    return stride;
}

void VertexLayout::add(uint32_t binding, Format attribute) {
    if (m_bindings.size() <= binding){
        m_bindings.resize(binding + 1);
    }
    m_bindings[binding].attributes.emplace_back(attribute);
}

void VertexLayout::add(const VertexInputBindingDescription& binding) {
    m_bindings.emplace_back(binding);
}

}