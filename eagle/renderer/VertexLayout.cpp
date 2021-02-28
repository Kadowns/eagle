//
// Created by Novak on 18/07/2019.
//

#include <utility>

#include "VertexLayout.h"

namespace eagle {

VertexLayout::VertexLayout() : m_elementCount(0) {

}

VertexLayout::VertexLayout(size_t elementCount, std::vector<eagle::Format> components) :
    m_components(std::move(components)), m_elementCount(elementCount) {
}

VertexLayout::VertexLayout(size_t elementCount, std::initializer_list<Format> components) :
        VertexLayout(elementCount, std::vector<Format>(components)) {
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

size_t VertexLayout::get_element_count() {
    return m_elementCount;
}



VertexLayout::~VertexLayout() = default;


}