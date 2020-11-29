//
// Created by Novak on 02/04/2020.
//

#include <eagle/core/renderer/vulkan/VulkanStorageBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanCleaner.h>

EG_BEGIN

VulkanStorageBuffer::VulkanStorageBuffer(VulkanStorageBufferCreateInfo createInfo, size_t size, void *data, BufferUsage usage) :
    m_createInfo(createInfo),
    StorageBuffer(size, usage) {
    if (data != nullptr){
        EG_CORE_DEBUG_F("Copying data to bytes array: data size: {0} | bytes size: {1}", size, m_bytes.size());
        memcpy(m_bytes.data(), data, size);
    }else{
        EG_CORE_DEBUG_F("Setting bytes to 0, no data set: data size: {0} | bytes size: {1}", size, m_bytes.size());
        memset(m_bytes.data(), 0, size);
        if (m_usage == BufferUsage::CONSTANT){
            EG_CORE_WARNING("Storage buffer created with CONSTANT usage flag with no data!");
        }
    }

    create_storage_buffer();
}

VulkanStorageBuffer::~VulkanStorageBuffer() {
    cleanup();
}

void VulkanStorageBuffer::set_data(void *data, size_t size, size_t offset) {
    assert(m_usage == BufferUsage::DYNAMIC);
    assert(size + offset <= m_bytes.size());
    memcpy(m_bytes.data() + offset, data, size);
    m_dirtyBytes = true;
}

void VulkanStorageBuffer::push() {
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

void VulkanStorageBuffer::create_storage_buffer() {
    EG_CORE_TRACE("Creating vulkan storage buffer impl");
    if (!m_cleared) return;

    m_buffers.resize(m_createInfo.bufferCount);

    switch(m_usage) {
        case BufferUsage::DYNAMIC: {

            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            for (auto &buffer : m_buffers) {
                VulkanBuffer::create_buffer(m_createInfo.physicalDevice, m_createInfo.device, buffer, createBufferInfo,
                                            m_bytes.size());
                buffer->map();
                buffer->copy_to(m_bytes.data(), m_bytes.size());
                buffer->flush();
            }
        }
            break;
        case BufferUsage::CONSTANT: {
            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            Reference<VulkanBuffer> stagingBuffer;
            VK_CALL
            VulkanBuffer::create_buffer(
                    m_createInfo.physicalDevice, m_createInfo.device, stagingBuffer, createBufferInfo, m_bytes.size(),
                    m_bytes.data());


            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

            for (int i = 0; i < m_buffers.size(); i++) {
                VK_CALL
                VulkanBuffer::create_buffer(m_createInfo.physicalDevice, m_createInfo.device, m_buffers[i],
                                            createBufferInfo, m_bytes.size());

                VK_CALL
                VulkanBuffer::copy_buffer(
                        m_createInfo.device,
                        m_createInfo.commandPool,
                        m_createInfo.graphicsQueue,
                        stagingBuffer->native_buffer(),
                        m_buffers[i]->native_buffer(),
                        m_bytes.size(),
                        0);
            }
            stagingBuffer->destroy();
            break;
        }
    }
    m_cleared = false;
    EG_CORE_TRACE("Vulkan storage buffer created");
}

bool VulkanStorageBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanStorageBuffer::flush(uint32_t index) {
    if (!is_dirty()) {
        return;
    }
    m_buffers[index]->copy_to(m_bytes.data(), m_bytes.size());
    m_dirtyBuffers.erase(index);
}

void VulkanStorageBuffer::cleanup() {
    if (m_cleared) return;
    for (size_t i = 0; i < m_buffers.size(); i++) {
        m_buffers[i]->unmap();
        m_buffers[i]->destroy();
    }
    m_dirtyBuffers.clear();
    m_cleared = true;
}

EG_END
