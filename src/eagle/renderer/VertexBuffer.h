//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VERTEXBUFFER_H
#define EAGLE_VERTEXBUFFER_H

#include <vector>
#include "eagle/core/Core.h"

_EAGLE_BEGIN

class VertexBuffer {
public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() = default;

    virtual const std::vector<float>& get_vertices() = 0;
    virtual const void* get_vertices_data() = 0;
    virtual uint32_t get_vertices_count() = 0;
    virtual uint32_t get_stride() = 0;

};

_EAGLE_END

#endif //EAGLE_VERTEXBUFFER_H
