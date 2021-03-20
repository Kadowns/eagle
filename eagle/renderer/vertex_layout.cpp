//
// Created by Novak on 18/07/2019.
//

#include <utility>

#include "vertex_layout.h"

namespace eagle {

VertexLayout::VertexLayout() = default;

VertexLayout::VertexLayout(std::vector<eagle::Format> components) :
    m_components(std::move(components)){
}

VertexLayout::VertexLayout(std::initializer_list<Format> components) :
        VertexLayout(std::vector<Format>(components)) {
}

size_t VertexLayout::get_stride() {
    size_t stride = 0;
    for (auto& component : m_components){
        stride += format_size(component);
    }
    return stride;
}

size_t VertexLayout::get_component_count() {
    return m_components.size();
}

std::vector<Format> &VertexLayout::get_components() {
    return m_components;
}

Format VertexLayout::get_component(uint32_t index) {
    return m_components[index];
}

VertexLayout::~VertexLayout() = default;


}