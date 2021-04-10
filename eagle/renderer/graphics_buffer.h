//
// Created by Ricardo on 4/5/2021.
//

#ifndef EAGLE_BUFFER_H
#define EAGLE_BUFFER_H

#include <cstdint>

namespace eagle {

class GraphicsBuffer {
public:
    GraphicsBuffer() = default;
    virtual ~GraphicsBuffer();

    inline void insert(void* data, uint32_t size) { copy_from(data, size, m_size); }

    void copy_from(void* data, uint32_t size, uint32_t offset = 0);
    void reserve(uint32_t size);
    void clear();

    uint32_t size() const { return m_size; }
    uint32_t capacity() const { return m_capacity; }

    virtual void upload() = 0;
protected:
    uint8_t* m_data = nullptr;
    uint32_t m_size = 0;
    uint32_t m_capacity = 0;

};

}

#endif //EAGLE_BUFFER_H
