//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <eagle/renderer/graphics_buffer.h>
#include <eagle/renderer/renderer_global_definitions.h>

namespace eagle {

enum class IndexBufferType{
    UINT_16 = 2,
    UINT_32 = 4
};

struct IndexBufferCreateInfo {
    UpdateType updateType;
    IndexBufferType indexType;
    uint32_t size;
    void* data;
};

class IndexBuffer : public GraphicsBuffer {
public:
    explicit IndexBuffer(const IndexBufferCreateInfo& createInfo) : m_createInfo(createInfo) {}
    ~IndexBuffer() override = default;

    inline IndexBufferType index_type() const { return m_createInfo.indexType; }
protected:
    IndexBufferCreateInfo m_createInfo;
};

}


#endif //EAGLE_INDEXBUFFER_H
