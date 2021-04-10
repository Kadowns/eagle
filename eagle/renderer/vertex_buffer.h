//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VERTEXBUFFER_H
#define EAGLE_VERTEXBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/graphics_buffer.h>

namespace eagle {

struct VertexBufferCreateInfo {
    UpdateType updateType;
};

class VertexBuffer : public GraphicsBuffer {
public:
    VertexBuffer(const VertexBufferCreateInfo& createInfo) : m_createInfo(createInfo) {}
    ~VertexBuffer() override = default;
protected:
    VertexBufferCreateInfo m_createInfo;
};

}

#endif //EAGLE_VERTEXBUFFER_H
