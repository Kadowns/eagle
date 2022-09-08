//
// Created by Novak on 06/06/2019.
//

#include "eagle/log.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_helper.h"

namespace eagle {

VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferCreateInfo &createInfo, const VulkanVertexBufferCreateInfo &vulkanCreateInfo, void *data, size_t size) :
        VertexBuffer(createInfo, data, size),
        m_vulkanInfo(vulkanCreateInfo),
        m_buffers(vulkanCreateInfo.frameCount) {
    if (size > 0){
        for (uint32_t i = 0; i < m_buffers.size(); i++){
            VulkanVertexBuffer::flush(i);
        }
    }
}


VulkanVertexBuffer::~VulkanVertexBuffer() {
    for (int i = 0; i < m_buffers.size(); i++){
        if (m_buffers[i]){
            m_buffers[i]->unmap();
            m_buffers[i].reset();
        }
    }
}

void VulkanVertexBuffer::flush(uint32_t bufferIndex) {

    VkDeviceSize bufferSize = m_size;
    auto& buffer = m_buffers[bufferIndex];
    if (!buffer || buffer->size() < bufferSize){
        switch(m_info.updateType){
            case UpdateType::BAKED:
                buffer = VulkanHelper::create_baked_buffer(
                        m_vulkanInfo.physicalDevice,
                        m_vulkanInfo.device,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        m_data,
                        bufferSize,
                        m_vulkanInfo.queue
                        );
                break;
            case UpdateType::DYNAMIC:
                buffer = VulkanHelper::create_dynamic_buffer(
                        m_vulkanInfo.physicalDevice,
                        m_vulkanInfo.device,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
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
                        m_vulkanInfo.queue,
                        m_data);
                break;
            case UpdateType::DYNAMIC:
                VulkanHelper::upload_dynamic_buffer(buffer, bufferSize, m_data);
                break;
        }
    }

    m_dirtyBuffers.erase(bufferIndex);
}

void VulkanVertexBuffer::flush() {
    for (int i = 0; i < m_buffers.size(); i++){
        m_dirtyBuffers.insert(i);
    }
    VulkanCleaner::push(this);
}

bool VulkanVertexBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

}


