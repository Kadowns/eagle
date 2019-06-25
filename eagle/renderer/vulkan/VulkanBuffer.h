//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANBUFFER_H
#define EAGLE_VULKANBUFFER_H

#include <memory>

#include "VulkanCore.h"

_EAGLE_BEGIN

struct VulkanBufferCreateInfo {

    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
};

class VulkanBuffer {

public:

    VulkanBuffer() = default;
    VulkanBuffer(VkDevice device, const VulkanBufferCreateInfo& createInfo);
    ~VulkanBuffer();

    VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
    VkResult bind(VkDeviceSize offset = 0);
    void unmap();
    void copy_to(void* data, VkDeviceSize size);
    void destroy();

    inline VkBuffer&       get_native_buffer() { return m_buffer; }
    inline VkDeviceMemory& get_memory()        { return m_memory; }
    inline void*           get_data()          { return m_mapped; }


    static VkResult
    create_buffer(VkPhysicalDevice physicalDevice, VkDevice device, std::shared_ptr<VulkanBuffer> &buffer,
                  const VulkanBufferCreateInfo &info, VkDeviceSize size, void *data = nullptr);

    static void
    copy_buffer(
            VkDevice device,
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            VkDeviceSize size,
            VkDeviceSize offset
    );
private:

    VkDevice m_device = VK_NULL_HANDLE;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    void* m_mapped = nullptr;

    VkBufferUsageFlags m_usageFlags = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    VkMemoryPropertyFlags m_memoryFlags = VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
};

_EAGLE_END


#endif //EAGLE_VULKANBUFFER_H
