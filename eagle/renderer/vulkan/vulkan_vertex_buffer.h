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

namespace eagle {

struct VulkanVertexBufferCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanVertexBuffer : public VertexBuffer, public VulkanCleanable {

public:

    VulkanVertexBuffer(const VertexBufferCreateInfo& createInfo, const VulkanVertexBufferCreateInfo &vulkanCreateInfo);
    ~VulkanVertexBuffer() override;

    //vulkan cleanable
    bool is_dirty() const override;
    void flush(uint32_t bufferIndex) override;

    //vertex buffer
    void upload() override;

    inline VulkanBuffer& native_buffer(uint32_t bufferIndex) {
        return *(m_buffers[bufferIndex]);
    }

private:
    VulkanVertexBufferCreateInfo m_vulkanCreateInfo;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
};

}

#endif //EAGLE_VULKANVERTEXBUFFER_H
