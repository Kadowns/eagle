//
// Created by Novak on 07/06/2019.
//

#include "vulkan_index_buffer.h"
#include "eagle/log.h"
#include "vulkan_helper.h"

namespace eagle {

VulkanIndexBuffer::VulkanIndexBuffer(const IndexBufferCreateInfo& createInfo, const VulkanIndexBufferCreateInfo& vulkanCreateInfo, void* data, size_t size) :
        IndexBuffer(createInfo, data, size),
        m_vulkanCreateInfo(vulkanCreateInfo),
        m_buffers(vulkanCreateInfo.bufferCount) {

    if (size > 0){
        for (int i = 0; i < m_buffers.size(); i++){
            VulkanIndexBuffer::flush(i);
        }
    }
}

VulkanIndexBuffer::~VulkanIndexBuffer() {
    for (int i = 0; i < m_buffers.size(); i++){
        auto& buffer = m_buffers[i];
        if (buffer){
            m_vulkanCreateInfo.deleter.destroy_buffer(buffer, i);
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
        switch(m_info.updateType){
            case UpdateType::BAKED:
                buffer = VulkanHelper::create_baked_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        m_data,
                        bufferSize,
                        m_vulkanCreateInfo.commandPool,
                        m_vulkanCreateInfo.graphicsQueue);

                break;
            case UpdateType::DYNAMIC:
                buffer = VulkanHelper::create_dynamic_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        m_data,
                        bufferSize);
                break;
        }
    }
    else {
        switch(m_info.updateType){
            case UpdateType::BAKED:
                VulkanHelper::upload_baked_buffer(
                        buffer,
                        m_vulkanCreateInfo.graphicsQueue,
                        m_vulkanCreateInfo.commandPool,
                        m_data);
                break;
            case UpdateType::DYNAMIC:
                VulkanHelper::upload_dynamic_buffer(buffer, bufferSize, m_data);
                break;
        }
    }

    m_dirtyBuffers.erase(bufferIndex);

}

void VulkanIndexBuffer::flush() {
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}
}
