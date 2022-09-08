//
// Created by Novak on 09/06/2019.
//

#include <algorithm>
#include "vulkan_uniform_buffer.h"

namespace eagle {

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data) :
    UniformBuffer(data, size),
    m_info(createInfo) {
    assert(size > 0);
    if (data != nullptr) {
        write(data, size);
    }
    else {
        memset(m_data, 0, size);
    }

    create_uniform_buffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    cleanup();
}

void VulkanUniformBuffer::flush(uint32_t bufferIndex) {

    if (!is_dirty()) {
        return;
    }
    m_buffers[bufferIndex]->copy_to(m_data, m_size);
    m_dirtyBuffers.erase(bufferIndex);
}

void VulkanUniformBuffer::create_uniform_buffer() {

    if (!m_cleared) return;

    m_buffers.resize(m_info.frameCount);

    VulkanBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferCreateInfo.data = m_data;
    bufferCreateInfo.size = m_size;
    bufferCreateInfo.device = m_info.device;
    bufferCreateInfo.physicalDevice = m_info.physicalDevice;

    for (auto& buffer : m_buffers) {
        buffer = std::make_shared<VulkanBuffer>(bufferCreateInfo);
        buffer->map(m_size);
    }
    m_cleared = false;
}

void VulkanUniformBuffer::cleanup() {
    if (m_cleared) return;
    for (auto& buffer : m_buffers) {
        buffer.reset();
    }
    m_dirtyBuffers.clear();
    m_cleared = true;
}

void VulkanUniformBuffer::flush() {
    m_dirtyBuffers.clear();
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}

bool VulkanUniformBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanUniformBuffer::recreate(uint32_t bufferCount) {
    m_info.frameCount = bufferCount;
    create_uniform_buffer();
}

}
