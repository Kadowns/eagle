//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VERTEXBUFFER_H
#define EAGLE_VERTEXBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/gpu_buffer.h>

namespace eagle {

struct VertexBufferCreateInfo {
    UpdateType updateType;
};

class VertexBuffer : public GPUBuffer {
public:
    explicit VertexBuffer(const VertexBufferCreateInfo& info) : m_info(info) {}
    VertexBuffer(const VertexBufferCreateInfo& info, void* data, size_t size) : GPUBuffer(data, size), m_info(info) {}
protected:
    VertexBufferCreateInfo m_info;
};

}

#endif //EAGLE_VERTEXBUFFER_H
