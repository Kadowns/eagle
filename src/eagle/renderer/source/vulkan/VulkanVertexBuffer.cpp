//
// Created by Novak on 06/06/2019.
//

#include "eagle/renderer/vulkan/VulkanVertexBuffer.h"

_EAGLE_BEGIN

VulkanVertexBuffer::VulkanVertexBuffer(VkDevice device, VulkanVertexBufferCreateInfo &createInfo) :
    m_device(device),
    m_vertices(createInfo.vertices),
    m_stride(createInfo.stride){

    VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

    VulkanBufferCreateInfo createBufferInfo = {};
    createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    std::shared_ptr<VulkanBuffer> stagingBuffer;
    VK_CALL
    VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, stagingBuffer, createBufferInfo,
                                bufferSize, m_vertices.data());


    createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VK_CALL
    VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, m_buffer, createBufferInfo, bufferSize,
                                nullptr);

    VK_CALL VulkanBuffer::copy_buffer(
            device,
            createInfo.commandPool,
            createInfo.graphicsQueue,
            stagingBuffer->get_native_buffer(),
            m_buffer->get_native_buffer(),
            bufferSize,
            0);

    stagingBuffer->destroy();
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    m_buffer->destroy();
}

const std::vector<float>&
VulkanVertexBuffer::get_vertices() {
    return m_vertices;
}

uint32_t
VulkanVertexBuffer::get_vertices_count() {
    return m_vertices.size() / m_stride;
}

uint32_t
VulkanVertexBuffer::get_stride() {
    return m_stride;
}

const void*
VulkanVertexBuffer::get_vertices_data() {
    return m_vertices.data();
}


_EAGLE_END


