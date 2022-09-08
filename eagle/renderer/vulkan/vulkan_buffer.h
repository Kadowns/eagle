//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANBUFFER_H
#define EAGLE_VULKANBUFFER_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_shared_command_pool.h>

namespace eagle {

class VulkanQueue;

struct VulkanBufferCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    VkDeviceSize size;
    void* data;
};

class VulkanBuffer {
public:

    VulkanBuffer(const VulkanBufferCreateInfo& info);
    ~VulkanBuffer();

    const VulkanBufferCreateInfo& info() const;
    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult bind(VkDeviceSize offset = 0);
    void unmap();
    void copy_to(void *data, VkDeviceSize size);
    void flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

    inline VkBuffer&       native_buffer() { return m_buffer; }
    inline VkDeviceMemory& memory()        { return m_memory; }
    inline void*           data()          { return m_mapped; }

    static void
    copy_buffer(VulkanQueue* queue, VulkanBuffer* src, VulkanBuffer* dst,
                VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset);

    VkDeviceSize size() const { return m_size; }

private:
    VulkanBufferCreateInfo m_info;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    void* m_mapped = nullptr;
    VkDeviceSize m_alignment = 0;
    VkDeviceSize m_size = 0;
};

}


#endif //EAGLE_VULKANBUFFER_H
