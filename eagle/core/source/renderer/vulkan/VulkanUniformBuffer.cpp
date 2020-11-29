//
// Created by Novak on 09/06/2019.
//

#include <algorithm>
#include "eagle/core/renderer/vulkan/VulkanUniformBuffer.h"

EG_BEGIN

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data) :
        UniformBuffer(size), m_info(createInfo) {

    if (data != nullptr){
        memcpy(m_bytes.data(), data, size);
    }else{
        memset(m_bytes.data(), 0, size);
    }

    create_uniform_buffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    cleanup();
}

void
VulkanUniformBuffer::flush(uint32_t bufferIndex) {

    if (!is_dirty()) {
        return;
    }
    m_buffers[bufferIndex]->copy_to(m_bytes.data(), m_bytes.size());
    m_dirtyBuffers.erase(bufferIndex);
}

void VulkanUniformBuffer::create_uniform_buffer() {

    if (!m_cleared) return;

    m_buffers.resize(m_info.bufferCount);

    VulkanBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i = 0; i < m_info.bufferCount; i++) {
        VulkanBuffer::create_buffer(m_info.physicalDevice, m_info.device, m_buffers[i],
                                    bufferCreateInfo, m_bytes.size(), m_bytes.data());
        m_buffers[i]->map(m_bytes.size());
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

void VulkanUniformBuffer::push() {
    if (!m_dirtyBytes){
        return;
    }

    m_dirtyBuffers.clear();
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
    m_dirtyBytes = false;
}

bool VulkanUniformBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanUniformBuffer::set_data(void *data, size_t size, size_t offset) {
    assert(size + offset <= m_bytes.size());
    memcpy(m_bytes.data() + offset, data, size);
    m_dirtyBytes = true;
}


EG_END
