//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VERTEXBUFFER_H
#define EAGLE_VERTEXBUFFER_H

#include <vector>
#include "eagle/core/CoreGlobalDefinitions.h"
#include "RenderingCore.h"

EG_BEGIN


class VertexBuffer {
public:
    VertexBuffer() = default;
    virtual ~VertexBuffer() = default;

    virtual uint32_t get_vertices_count() = 0;
    virtual uint32_t get_stride() = 0;
    virtual void upload(void* data, uint32_t elementCount) = 0;

};

EG_END

#endif //EAGLE_VERTEXBUFFER_H
