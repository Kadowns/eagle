//
// Created by Novak on 09/06/2019.
//

#include <algorithm>
#include "eagle/renderer/vulkan/VulkanUniformBuffer.h"

_EAGLE_BEGIN

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void* data) :
        UniformBuffer(size), m_info(createInfo), m_data(data) {
    create_uniform_buffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    cleanup();
}

void
VulkanUniformBuffer::flush(uint32_t bufferIndex) {
    m_buffers[bufferIndex]->copy_to(m_data, m_size);
    if (is_dirty()){
        m_dirtyBuffers.erase(std::find(m_dirtyBuffers.begin(), m_dirtyBuffers.end(), bufferIndex));
    }
}

void VulkanUniformBuffer::create_uniform_buffer() {

    if (!m_cleared) return;

    m_buffers.resize(m_info.bufferCount);

    VulkanBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i = 0; i < m_info.bufferCount; i++) {
        VulkanBuffer::create_buffer(m_info.physicalDevice, m_info.device, m_buffers[i],
                                    bufferCreateInfo, m_size, m_data);
        m_buffers[i]->map(m_size);
    }
    m_cleared = false;
}

void VulkanUniformBuffer::cleanup() {
    if (m_cleared) return;
    for (size_t i = 0; i < m_buffers.size(); i++) {
        m_buffers[i]->unmap();
        m_buffers[i]->destroy();
    }
    m_dirtyBuffers.clear();
    m_cleared = true;
}

void VulkanUniformBuffer::upload_data(void *data) {
    m_data = data;
    m_dirtyBuffers.resize(m_buffers.size());
    for (size_t i = 0; i < m_dirtyBuffers.size(); i++){
        m_dirtyBuffers[i] = i;
    }
}

bool VulkanUniformBuffer::is_dirty() {
    return !m_dirtyBuffers.empty();
}


_EAGLE_END


