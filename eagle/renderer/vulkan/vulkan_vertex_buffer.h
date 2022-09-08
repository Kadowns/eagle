//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXBUFFER_H
#define EAGLE_VULKANVERTEXBUFFER_H

#include <eagle/renderer/vertex_buffer.h>
#include <eagle/renderer/vertex_layout.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_cleaner.h>

#include <set>

namespace eagle {

class VulkanSharedCommandPool;

struct VulkanVertexBufferCreateInfo {
    VkDevice device;
    uint32_t frameCount;
    VkPhysicalDevice physicalDevice;
    VulkanQueue* queue;
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

    inline VulkanBuffer& internal_buffer(uint32_t bufferIndex) {
        return *(m_buffers[bufferIndex]);
    }

private:
    VulkanVertexBufferCreateInfo m_vulkanInfo;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<uint32_t> m_dirtyBuffers;
};

}

#endif //EAGLE_VULKANVERTEXBUFFER_H
