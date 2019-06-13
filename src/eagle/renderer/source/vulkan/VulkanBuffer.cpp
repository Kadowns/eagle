//
// Created by Novak on 06/06/2019.
//
#include <assert.h>
#include "eagle/renderer/vulkan/VulkanBuffer.h"
#include "eagle/renderer/vulkan/VulkanHelper.h"

#include "eagle/core/Log.h"

_EAGLE_BEGIN

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
VulkanBuffer::copy_to(void* data, VkDeviceSize size) {
    assert(m_mapped);
    memcpy(m_mapped, data, size);
}

VkResult
VulkanBuffer::create_buffer(VkPhysicalDevice physicalDevice, VkDevice device, std::shared_ptr<VulkanBuffer> &buffer,
                            VkDeviceSize size,
                            const VulkanBufferCreateInfo &info, void *data) {
    buffer = std::make_shared<VulkanBuffer>(device, info);
    VkResult result;
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = info.usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if ((result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer->get_native_buffer())) != VK_SUCCESS) {
        return result;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer->get_native_buffer(), &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            physicalDevice, memRequirements.memoryTypeBits, info.memoryFlags
            );

    //TODO --- implementar solução top de alocação de memória (ver vulkan tutorial)
    if ((result = vkAllocateMemory(device, &allocInfo, nullptr, &buffer->get_memory())) != VK_SUCCESS) {
        return result;
    }

    if (data != nullptr) {
        if ((result = buffer->map()) != VK_SUCCESS) {
            return result;
        }
        memcpy(buffer->get_data(), data, size);
        buffer->unmap();
    }

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

    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(device, commandPool);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = offset; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VulkanHelper::end_single_time_commnds(device, commandPool, commandBuffer, graphicsQueue);

}


_EAGLE_END


