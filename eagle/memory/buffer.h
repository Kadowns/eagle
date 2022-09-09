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
    void write(const T& obj){
        write_data((void*)&obj, sizeof(obj), 0);
    }

    void write_data(void* data, size_t size, size_t offset = 0);
    void reserve(size_t size);
    void clear();

    uint32_t size() const { return m_size; }

protected:
    uint8_t* m_data = nullptr;
    size_t m_size = 0;
};

}

#endif //EAGLE_BUFFER_H
