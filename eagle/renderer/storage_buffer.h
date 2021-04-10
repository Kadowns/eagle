//
// Created by Novak on 01/04/2020.
//

#ifndef EAGLE_STORAGEBUFFER_H
#define EAGLE_STORAGEBUFFER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/descriptor_item.h>

namespace eagle {

class StorageBuffer : public DescriptorItem {
public:

    explicit StorageBuffer(size_t size, UpdateType usage) : m_bytes(size), m_usage(usage) {}
    virtual ~StorageBuffer() = default;

    virtual void copy_from(void *data, size_t size, size_t offset) = 0;
    virtual void upload() = 0;

    inline size_t size() const {return m_bytes.size();}
    inline const std::vector<char>& data() const { return m_bytes; }
protected:
    UpdateType m_usage;
    std::vector<char> m_bytes;
};

}

#endif //EAGLE_STORAGEBUFFER_H
