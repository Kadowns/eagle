//
// Created by Novak on 10/06/2019.
//

#include "eagle/renderer/UniformBuffer.h"

_EAGLE_BEGIN

UniformBuffer::UniformBuffer(const ShaderItemLayout& layout):
    m_layout(layout){

}

UniformBuffer::~UniformBuffer() = default;

_EAGLE_END