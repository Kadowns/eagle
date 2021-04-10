//
// Created by Novak on 07/06/2019.
//

#include "vulkan_index_buffer.h"
#include "eagle/log.h"
#include "vulkan_helper.h"

namespace eagle {

VulkanIndexBuffer::VulkanIndexBuffer(const IndexBufferCreateInfo& createInfo,
                                     VulkanIndexBufferCreateInfo& vulkanCreateInfo) :
        IndexBuffer(createInfo),
        m_vulkanCreateInfo(vulkanCreateInfo),
        m_buffers(vulkanCreateInfo.bufferCount) {
    if (createInfo.size == 0){
        return;
    }
    reserve(createInfo.size);
    copy_from(createInfo.data, createInfo.size);
    for (int i = 0; i < m_buffers.size(); i++){
        flush(i);
    }
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
    for (auto& buffer : m_buffers){
        if (buffer){
            buffer->unmap();
            buffer->destroy();
        }
    }
}

bool VulkanIndexBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

void VulkanIndexBuffer::flush(uint32_t bufferIndex) {

    VkDeviceSize bufferSize = m_size;
    auto& buffer = m_buffers[bufferIndex];
    if (!buffer || buffer->size() < bufferSize){
        switch(m_createInfo.updateType){
            case UpdateType::BAKED:
                VulkanHelper::create_baked_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        m_vulkanCreateInfo.graphicsQueue,
                        m_vulkanCreateInfo.commandPool,
                        buffer,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        bufferSize,
                        m_data);
                break;
            case UpdateType::DYNAMIC:
                VulkanHelper::create_dynamic_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device, buffer,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        bufferSize,
                        m_data);
                break;
        }
    }
    else {
        switch(m_createInfo.updateType){
            case UpdateType::BAKED:
                VulkanHelper::upload_baked_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        m_vulkanCreateInfo.graphicsQueue,
                        m_vulkanCreateInfo.commandPool,
                        buffer,
                        bufferSize,
                        m_data);
                break;
            case UpdateType::DYNAMIC:
                VulkanHelper::upload_dynamic_buffer(buffer, bufferSize, m_data);
                break;
        }
    }

    m_dirtyBuffers.erase(bufferIndex);

}

void VulkanIndexBuffer::upload() {
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}
}
