//
// Created by Novak on 07/06/2019.
//

#include "eagle/renderer/vulkan/VulkanIndexBuffer.h"

_EAGLE_BEGIN

VulkanIndexBuffer::VulkanIndexBuffer(VkDevice device, VulkanIndexBufferCreateInfo &createInfo):
        m_device(device),
        m_indices(createInfo.indices) {

    VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

    VulkanBufferCreateInfo createBufferInfo = {};
    createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VulkanBuffer stagingBuffer(m_device, createBufferInfo);
    VK_CALL VulkanBuffer::create_buffer(
            createInfo.physicalDevice, m_device, stagingBuffer, bufferSize, createBufferInfo, m_indices.data());


    createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;


    m_buffer = std::make_shared<VulkanBuffer>(device, createBufferInfo);
    VK_CALL VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, *m_buffer, bufferSize, createBufferInfo);

    VK_CALL VulkanBuffer::copy_buffer(
            device,
            createInfo.commandPool,
            createInfo.graphicsQueue,
            stagingBuffer.get_native_buffer(),
            m_buffer->get_native_buffer(),
            bufferSize,
            0);

    stagingBuffer.destroy();
}
VulkanIndexBuffer::~VulkanIndexBuffer() {
    m_buffer->destroy();
}

const std::vector<uint32_t>& VulkanIndexBuffer::get_indices() {
    return m_indices;
}

const void *VulkanIndexBuffer::get_indices_data() {
    return m_indices.data();
}

uint32_t VulkanIndexBuffer::get_indices_count() {
    return m_indices.size();
}

_EAGLE_END