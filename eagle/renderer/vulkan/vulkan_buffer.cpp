//
// Created by Novak on 06/06/2019.
//
#include <assert.h>
#include "vulkan_buffer.h"
#include "vulkan_helper.h"

#include "eagle/log.h"

namespace eagle {

VulkanBuffer::~VulkanBuffer() = default;

VulkanBuffer::VulkanBuffer(VkDevice device, const VulkanBufferCreateInfo& createInfo):
        m_device(device),
        m_usageFlags(createInfo.usageFlags),
        m_memoryFlags(createInfo.memoryFlags){

}

VkResult
VulkanBuffer::map(VkDeviceSize size, VkDeviceSize offset) {
    return vkMapMemory(m_device, m_memory, offset, size, 0, &m_mapped);
}

void
VulkanBuffer::unmap() {
    if (m_mapped) {
        vkUnmapMemory(m_device, m_memory);
        m_mapped = nullptr;
    }
}

VkResult
VulkanBuffer::bind(VkDeviceSize offset) {
    return vkBindBufferMemory(m_device, m_buffer, m_memory, offset);
}

void
VulkanBuffer::destroy() {
    if (m_buffer) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
    }

    if (m_memory) {
        vkFreeMemory(m_device, m_memory, nullptr);
    }
}

void
VulkanBuffer::copy_to(void *data, VkDeviceSize size) {
    assert(m_mapped);
    memcpy(m_mapped, data, size);
}

VkResult
VulkanBuffer::create_buffer(VkPhysicalDevice physicalDevice, VkDevice device, StrongPointer<VulkanBuffer> &buffer,
                            const VulkanBufferCreateInfo &info, VkDeviceSize size, void *data) {
    EG_TRACE("eagle","Creating vulkan buffer");
    buffer = make_strong<VulkanBuffer>(device, info);
    VkResult result;
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = info.usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if ((result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer->native_buffer())) != VK_SUCCESS) {
        EG_ERROR("eagle", "Failed to create vulkan buffer");
        return result;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer->native_buffer(), &memRequirements);

    buffer->m_alignment = memRequirements.alignment;
    buffer->m_size = memRequirements.size;
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            physicalDevice, memRequirements.memoryTypeBits, info.memoryFlags
            );

    //TODO --- implementar solução top de alocação de memória (ver vulkan tutorial)
    if ((result = vkAllocateMemory(device, &allocInfo, nullptr, &buffer->get_memory())) != VK_SUCCESS) {
        EG_ERROR("eagle", "Failed allocate memory for vulkan buffer");
        return result;
    }

    if (data != nullptr) {
        if ((result = buffer->map()) != VK_SUCCESS) {
            EG_ERROR("eagle", "Failed to map memory for vulkan buffer");
            return result;
        }
        memcpy(buffer->get_data(), data, size);
        buffer->unmap();
    }

    EG_TRACE("eagle","Vulkan buffer created");
    return buffer->bind();

}

void VulkanBuffer::copy_buffer(
        VkDevice device,
        VkCommandPool commandPool,
        VkQueue graphicsQueue,
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size,
        VkDeviceSize offset) {
    EG_TRACE("eagle","Copying vulkan buffer");
    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(device, commandPool);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = offset;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VulkanHelper::end_single_time_commnds(device, commandPool, commandBuffer, graphicsQueue);
    EG_TRACE("eagle","Vulkan buffer copied");
}

void VulkanBuffer::flush(VkDeviceSize size, VkDeviceSize offset) {
    VkMappedMemoryRange memoryRange = {};
    memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    memoryRange.size = size;
    memoryRange.offset = offset;
    memoryRange.memory = m_memory;
    vkFlushMappedMemoryRanges(m_device, 1, &memoryRange);
}


}


