//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXBUFFER_H
#define EAGLE_VULKANVERTEXBUFFER_H

#include <set>

#include "eagle/renderer/vertex_buffer.h"
#include "eagle/renderer/vertex_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_cleaner.h"
#include "vulkan_deleter.h"

namespace eagle {

struct VulkanVertexBufferCreateInfo {
    VulkanDeleter& deleter;
    VkDevice device;
    uint32_t bufferCount;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanVertexBuffer : public VertexBuffer, public VulkanCleanable {
public:

    VulkanVertexBuffer(const VertexBufferCreateInfo& createInfo, const VulkanVertexBufferCreateInfo &vulkanCreateInfo, void* data, size_t size);
    ~VulkanVertexBuffer() override;

    //vulkan cleanable
    bool is_dirty() const override;
    void flush(uint32_t bufferIndex) override;

    //vertex buffer
    void flush() override;

    inline VulkanBuffer& native_buffer(uint32_t bufferIndex) {
        return *(m_buffers[bufferIndex]);
    }

private:
    VulkanVertexBufferCreateInfo m_vulkanInfo;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<uint32_t> m_dirtyBuffers;
};

}

#endif //EAGLE_VULKANVERTEXBUFFER_H
