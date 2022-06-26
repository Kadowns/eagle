//
// Created by Ricardo on 25/06/2022.
//

#include <eagle/memory/buffer.h>

#include <cstring>

namespace eagle {

Buffer::Buffer(void *data, size_t size) {
    write(data, size);
}

Buffer::~Buffer() {
    delete[] m_data;
}

void Buffer::write(void *data, size_t size, size_t offset) {
    if (size + m_size > m_capacity){
        size_t diff = m_size + size - m_capacity;
        reserve(m_capacity + diff);
    }

    memcpy(m_data + offset, data, size);
    m_size += size;
}

void Buffer::reserve(size_t size) {
    if (size <= m_capacity) {
        return;
    }
    auto tmp = m_data;
    m_capacity = size;
    m_data = new uint8_t[size];
    if (tmp) {
        memcpy(m_data, tmp, m_size);
        delete[] tmp;
    }
}

void Buffer::clear() {
    m_size = 0;
}


}