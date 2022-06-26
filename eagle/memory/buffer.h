//
// Created by Ricardo on 25/06/2022.
//

#ifndef EAGLE_BUFFER_H
#define EAGLE_BUFFER_H

#include <cstdint>

namespace eagle {

class Buffer {
public:
    Buffer() = default;
    Buffer(void* data, size_t size);
    virtual ~Buffer();

    template<typename T>
    inline void write(const T& obj){
        write(&obj, sizeof(obj));
    }

    void write(void* data, size_t size, size_t offset = 0);
    void reserve(size_t size);
    void clear();

    uint32_t size() const { return m_size; }
    uint32_t capacity() const { return m_capacity; }

protected:
    uint8_t* m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;
};

}

#endif //EAGLE_BUFFER_H
