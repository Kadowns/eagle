//
// Created by Novak on 09/06/2019.
//

#include "eagle/renderer/vulkan/VulkanUniformBuffer.h"

_EAGLE_BEGIN

VulkanUniformBuffer::VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, const ShaderItemLayout &layout) :
        UniformBuffer(layout), m_info(createInfo) {
    create_uniform_buffer();
}

VulkanUniformBuffer::~VulkanUniformBuffer() {
    cleanup();
}

void
VulkanUniformBuffer::flush(void *data, uint32_t bufferIndex) {
    m_buffers[bufferIndex]->copy_to(data, m_layout.stride());
}

size_t
VulkanUniformBuffer::size() {
    return m_layout.stride();
}

void VulkanUniformBuffer::create_uniform_buffer() {

    if (!m_cleared) return;

    m_buffers.resize(m_info.bufferCount);

    VulkanBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    for (size_t i = 0; i < m_info.bufferCount; i++) {
        VulkanBuffer::create_buffer(m_info.physicalDevice, m_info.device, m_buffers[i], m_layout.stride(),
                                    bufferCreateInfo);
        m_buffers[i]->map(m_layout.stride());
    }
    m_cleared = false;
}

void VulkanUniformBuffer::cleanup() {
    if (m_cleared) return;
    for (size_t i = 0; i < m_buffers.size(); i++) {
        m_buffers[i]->unmap();
        m_buffers[i]->destroy();
    }
    m_cleared = true;
}


_EAGLE_END


