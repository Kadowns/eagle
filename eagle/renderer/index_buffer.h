//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <eagle/renderer/gpu_buffer.h>
#include <eagle/renderer/renderer_global_definitions.h>

namespace eagle {

enum class IndexBufferType {
    UINT_16 = 2,
    UINT_32 = 4
};

struct IndexBufferCreateInfo {
    UpdateType updateType;
    IndexBufferType indexType;
};

class IndexBuffer : public GPUBuffer {
public:
    explicit IndexBuffer(const IndexBufferCreateInfo& info) : m_info(info) {}
    IndexBuffer(const IndexBufferCreateInfo& info, void* data, size_t size) : GPUBuffer(data, size), m_info(info) {}

    inline IndexBufferType index_type() const { return m_info.indexType; }
protected:
    IndexBufferCreateInfo m_info;
};

}


#endif //EAGLE_INDEXBUFFER_H
