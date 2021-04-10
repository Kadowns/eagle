//
// Created by Ricardo on 4/5/2021.
//

#include "graphics_buffer.h"

#include <memory>
#include <cassert>

namespace eagle {

GraphicsBuffer::~GraphicsBuffer() {
    delete[] m_data;
}

void GraphicsBuffer::copy_from(void* data, uint32_t size, uint32_t offset) {
    assert(offset + size <= m_capacity);
    memcpy(m_data + offset, data, size);
    m_size += size;
}

void GraphicsBuffer::reserve(uint32_t size) {
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

void GraphicsBuffer::clear() {
    m_size = 0;
}

}