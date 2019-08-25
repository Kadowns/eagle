//
// Created by Novak on 10/06/2019.
//

#include "eagle/core/renderer/UniformBuffer.h"

EG_BEGIN

UniformBuffer::UniformBuffer(size_t size):
    DescriptorItem(EG_DESCRIPTOR_TYPE::UNIFORM_BUFFER),
    m_size(size){

}

UniformBuffer::~UniformBuffer() = default;

EG_END