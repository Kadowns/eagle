//
// Created by Novak on 10/06/2019.
//

#include "eagle/renderer/UniformBuffer.h"

_EAGLE_BEGIN

UniformBuffer::UniformBuffer(Eagle::UNIFORM_BUFFER_USAGE usageFlags)
    : m_usageFlags(usageFlags) {

}

UniformBuffer::~UniformBuffer() = default;

_EAGLE_END