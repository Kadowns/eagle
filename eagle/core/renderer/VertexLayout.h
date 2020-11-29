//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_VERTEXLAYOUT_H
#define EAGLE_VERTEXLAYOUT_H

#include <initializer_list>
#include <vector>

#include "RenderingCore.h"

EG_BEGIN

class VertexLayout {

public:
    VertexLayout();
    VertexLayout(size_t elementCount, std::initializer_list<Format> components);
    VertexLayout(size_t elementCount, std::vector<Eagle::Format> components);
    ~VertexLayout();

    size_t get_stride();
    size_t get_component_count();
    size_t get_element_count();
    std::vector<Format>& get_components();
    Format get_component(uint32_t index);

    inline Format operator[](uint32_t index){
        return m_components[index];
    }

private:
    std::vector<Format> m_components;
    size_t m_elementCount;
};



EG_END

#endif //EAGLE_VERTEXLAYOUT_H
