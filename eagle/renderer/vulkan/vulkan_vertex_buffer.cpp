//
// Created by Novak on 06/06/2019.
//

#include "eagle/log.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_helper.h"

namespace eagle {

VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferCreateInfo& createInfo,
                                       const VulkanVertexBufferCreateInfo& vulkanCreateInfo) :
        VertexBuffer(createInfo),
        m_vulkanCreateInfo(vulkanCreateInfo),
        m_buffers(vulkanCreateInfo.bufferCount){
    if (createInfo.size == 0){
        return;
    }

    reserve(createInfo.size);
    copy_from(createInfo.data, createInfo.size);
    for (int i = 0; i < m_buffers.size(); i++){
        flush(i);
    }
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    for (int i = 0; i < m_buffers.size(); i++){
        if (m_buffers[i]){
            m_buffers[i]->unmap();
            m_vulkanCreateInfo.deleter.destroy_buffer(m_buffers[i], i);
        }
    }
}

void VulkanVertexBuffer::flush(uint32_t bufferIndex) {

    VkDeviceSize bufferSize = m_size;
    auto& buffer = m_buffers[bufferIndex];
    if (!buffer || buffer->size() < bufferSize){
        switch(m_createInfo.updateType){
            case UpdateType::BAKED:
                VulkanHelper::create_baked_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        m_data,
                        bufferSize,
                        m_vulkanCreateInfo.commandPool,
                        m_vulkanCreateInfo.graphicsQueue
                        );
                break;
            case UpdateType::DYNAMIC:
                VulkanHelper::create_dynamic_buffer(
                        m_vulkanCreateInfo.physicalDevice,
                        m_vulkanCreateInfo.device,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        m_data,
                        bufferSize);
                break;
        }
    }
    else {
        switch(m_createInfo.updateType){
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

void VulkanVertexBuffer::upload() {
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}

bool VulkanVertexBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

}


