//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_VERTEXLAYOUT_H
#define EAGLE_VERTEXLAYOUT_H

#include <initializer_list>
#include <vector>

#include "RenderingCore.h"

namespace eagle {

class VertexLayout {

public:
    VertexLayout();
    VertexLayout(std::initializer_list<Format> components);
    VertexLayout(std::vector<eagle::Format> components);
    ~VertexLayout();

    size_t get_stride();
    size_t get_component_count();
    std::vector<Format>& get_components();
    Format get_component(uint32_t index);

    inline Format operator[](uint32_t index){
        return m_components[index];
    }

private:
    std::vector<Format> m_components;
};



}

#endif //EAGLE_VERTEXLAYOUT_H
