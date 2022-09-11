//
// Created by Ricardo on 10/09/2022.
//

#include <eagle/renderer/command_buffer.h>

namespace eagle {

CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& createInfo) :
    m_createInfo(createInfo) {
}

CommandBuffer::~CommandBuffer() = default;

CommandQueue* CommandBuffer::command_queue() const {
    return m_createInfo.queue;
}

}
