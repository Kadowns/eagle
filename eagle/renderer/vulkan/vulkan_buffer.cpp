//
// Created by Novak on 06/06/2019.
//
#include <assert.h>
#include "vulkan_buffer.h"
#include "vulkan_helper.h"

#include "eagle/log.h"

namespace eagle {

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& info):
    m_info(info){
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = info.size;
    bufferInfo.usage = info.usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(info.device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        EG_ERROR("eagle", "Failed to create vulkan buffer");
        return;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(info.device, m_buffer, &memRequirements);

    m_alignment = memRequirements.alignment;
    m_size = memRequirements.size;
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            info.physicalDevice, memRequirements.memoryTypeBits, info.memoryFlags
    );

    //TODO --- implementar solução top de alocação de memória (ver vulkan tutorial)
    if (vkAllocateMemory(info.device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        EG_ERROR("eagle", "Failed allocate memory for vulkan buffer");
        return;
    }

    if (info.data != nullptr) {
        if (map() != VK_SUCCESS) {
            EG_ERROR("eagle", "Failed to map memory for vulkan buffer");
            return;
        }
        memcpy(m_mapped, info.data, info.size);
        unmap();
    }

    bind();
}

VulkanBuffer::~VulkanBuffer() {

    unmap();

    if (m_buffer) {
        vkDestroyBuffer(m_info.device, m_buffer, nullptr);
    }

    if (m_memory) {
        vkFreeMemory(m_info.device, m_memory, nullptr);
    }
}

VkResult
VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
    return vkMapMemory(m_info.device, m_memory, offset, size, 0, &m_mapped);
}

void
VulkanBuffer::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_info.device, m_memory);
        m_mapped = nullptr;
    }
}

VkResult
VulkanBuffer::bind(VkDeviceSize offset) {
    return vkBindBufferMemory(m_info.device, m_buffer, m_memory, offset);
}

void
VulkanBuffer::copy_to(void *data, VkDeviceSize size) {
    assert(m_mapped);
    memcpy(m_mapped, data, size);
}

void VulkanBuffer::copy_buffer(VulkanCommandQueue* queue, VulkanBuffer* src, VulkanBuffer* dst,
                               VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
    EG_TRACE("eagle","Copying vulkan buffer");
    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(queue);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src->m_buffer, dst->m_buffer, 1, &copyRegion);

    VulkanHelper::end_single_time_commnds(queue, commandBuffer);
}

void VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.size = size;
    memoryRange.offset = offset;
    memoryRange.memory = m_memory;
    vkFlushMappedMemoryRanges(m_info.device, 1, &memoryRange);
}

const VulkanBufferCreateInfo& VulkanBuffer::info() const {
    return m_info;
}

}


