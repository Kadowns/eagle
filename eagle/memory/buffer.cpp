//
// Created by Ricardo on 25/06/2022.
//

#include <eagle/memory/buffer.h>

#include <cstring>
#include <cassert>

namespace eagle {

Buffer::Buffer(void *data, size_t size) {
    reserve(size);
    if (data){
        write_data(data, size);
    }
}

Buffer::~Buffer() {
    delete[] m_data;
}

void Buffer::write_data(void* data, size_t size, size_t offset) {
    assert(size + offset <= m_size);
    memcpy(m_data + offset, data, size);
}

void Buffer::reserve(size_t size) {
    if (size <= m_size) {
        return;
    }
    auto tmp = m_data;
    m_size = size;
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