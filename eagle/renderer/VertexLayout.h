//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_VERTEXLAYOUT_H
#define EAGLE_VERTEXLAYOUT_H

#include <initializer_list>
#include <vector>

#include "RenderingCore.h"

_EAGLE_BEGIN

class VertexLayout {

public:
    VertexLayout(size_t elementCount, std::initializer_list<EG_FORMAT> components);
    VertexLayout(size_t elementCount, std::vector<Eagle::EG_FORMAT> components);
    ~VertexLayout();

    size_t get_stride();
    size_t get_component_count();
    size_t get_element_count();
    std::vector<EG_FORMAT>& get_components();
    EG_FORMAT get_component(uint32_t index);

    inline EG_FORMAT operator[](uint32_t index){
        return m_components[index];
    }

private:
    std::vector<EG_FORMAT> m_components;
    size_t m_elementCount;
};



_EAGLE_END

#endif //EAGLE_VERTEXLAYOUT_H
