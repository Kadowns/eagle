//
// Created by Novak on 07/06/2019.
//

#include "eagle/core/renderer/vulkan/VulkanIndexBuffer.h"
#include "eagle/core/Log.h"

EG_BEGIN

VulkanIndexBuffer::VulkanIndexBuffer(VkDevice device, VulkanIndexBufferCreateInfo &createInfo, void *indexData,
                                     size_t indexCount,
                                     IndexBufferType indexType, BufferUsage usage) :
        m_device(device),
        m_physicalDevice(createInfo.physicalDevice),
        m_indexCount(indexCount),
        m_indexType(indexType),
        m_usage(usage),
        m_buffers(createInfo.bufferCount){

    VkDeviceSize bufferSize = static_cast<int>(indexType) * indexCount;

    if (indexCount != 0){
        m_data = new char[bufferSize];
        memcpy(m_data, indexData, bufferSize);
    }

    switch(m_usage){
        case BufferUsage::DYNAMIC:{

            if (indexCount == 0) {
                return;
            }

            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
            for (auto& buffer : m_buffers){
                VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, buffer, createBufferInfo, bufferSize);
                buffer->map();
                buffer->copy_to(m_data, bufferSize);
                buffer->flush();
            }
        } break;
        case BufferUsage::CONSTANT:{
            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            Reference<VulkanBuffer> stagingBuffer;
            VK_CALL
            VulkanBuffer::create_buffer(
                    createInfo.physicalDevice, m_device, stagingBuffer, createBufferInfo, bufferSize, m_data);


            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

            VK_CALL
            VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, m_buffers[0], createBufferInfo, bufferSize);

            VK_CALL VulkanBuffer::copy_buffer(
                    device,
                    createInfo.commandPool,
                    createInfo.graphicsQueue,
                    stagingBuffer->native_buffer(),
                    m_buffers[0]->native_buffer(),
                    bufferSize,
                    0);

            stagingBuffer->destroy();
        } break;
    }
    m_initialized = true;
}
VulkanIndexBuffer::~VulkanIndexBuffer() {
    for (auto& buffer : m_buffers){
        buffer->unmap();
        buffer->destroy();
    }
}

void* VulkanIndexBuffer::get_data() {
    return m_data;
}

size_t VulkanIndexBuffer::get_indices_count() {
    return m_indexCount;
}


bool VulkanIndexBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanIndexBuffer::flush(uint32_t bufferIndex) {

    if (!is_dirty()){
        return;
    }

    VkDeviceSize bufferSize = m_indexCount * static_cast<int>(m_indexType);

    if (m_resizing || !m_buffers[bufferIndex]){
        if (m_buffers[bufferIndex]){
            m_buffers[bufferIndex]->unmap();
            m_buffers[bufferIndex]->destroy();
        }


        VulkanBufferCreateInfo createBufferInfo = {};
        createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        createBufferInfo.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        VulkanBuffer::create_buffer(m_physicalDevice, m_device, m_buffers[bufferIndex], createBufferInfo, bufferSize, m_data);
        m_buffers[bufferIndex]->map();
    }
    m_buffers[bufferIndex]->copy_to(m_data, bufferSize);
    m_buffers[bufferIndex]->flush();
    m_dirtyBuffers.erase(bufferIndex);

}

void VulkanIndexBuffer::upload(void *data, uint32_t indexCount) {
    if (m_usage == BufferUsage::CONSTANT){
        EG_CORE_ERROR("Trying to flush data to a index buffer created with CONSTANT flag! To be able to flush data, be sure to create the buffer with DYNAMIC flag!");
        return;
    }

    VkDeviceSize bufferSize = static_cast<int>(m_indexType) * indexCount;

    if (m_data != nullptr){
        delete[](m_data);
    }

    m_data = new char[bufferSize];
    memcpy(m_data, data, bufferSize);


    if (!m_initialized){

        VulkanBufferCreateInfo createBufferInfo = {};
        createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        createBufferInfo.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        for (auto& buffer : m_buffers){
            VulkanBuffer::create_buffer(m_physicalDevice, m_device, buffer, createBufferInfo, bufferSize);
            buffer->map();
            buffer->copy_to(m_data, bufferSize);
            buffer->flush();
        }
        m_initialized = true;
    }
    else{
        if (m_indexCount != indexCount){
            m_resizing = true;
        }

        m_dirtyBuffers.clear();
        for (int i = 0; i < m_buffers.size(); i++){
            m_dirtyBuffers.insert(i);
        }
        VulkanCleaner::push(this);
    }
    m_indexCount = indexCount;
}

EG_END