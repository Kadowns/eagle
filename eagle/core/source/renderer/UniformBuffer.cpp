//
// Created by Novak on 10/06/2019.
//

#include "eagle/core/renderer/UniformBuffer.h"

EG_BEGIN

UniformBuffer::UniformBuffer(size_t size):
    DescriptorItem(DescriptorType::UNIFORM_BUFFER), m_bytes(size){

}

UniformBuffer::~UniformBuffer() = default;

EG_END