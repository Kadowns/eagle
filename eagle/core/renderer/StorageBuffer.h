//
// Created by Novak on 01/04/2020.
//

#ifndef EAGLE_STORAGEBUFFER_H
#define EAGLE_STORAGEBUFFER_H

#include <eagle/core/renderer/RenderingCore.h>
#include <eagle/core/renderer/DescriptorItem.h>

EG_BEGIN

class StorageBuffer : public DescriptorItem {
public:

    explicit StorageBuffer(size_t size, BufferUsage usage) : DescriptorItem(DescriptorType::STORAGE_BUFFER), m_bytes(size), m_usage(usage) {}
    virtual ~StorageBuffer() = default;

    virtual void set_data(void *data, size_t size, size_t offset) = 0;
    virtual void push() = 0;

    inline size_t size() const {return m_bytes.size();}
    inline const std::vector<char>& data() const { return m_bytes; }
protected:
    BufferUsage m_usage;
    std::vector<char> m_bytes;
};

EG_END

#endif //EAGLE_STORAGEBUFFER_H
