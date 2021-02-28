//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXBUFFER_H
#define EAGLE_VULKANVERTEXBUFFER_H

#include <set>

#include "eagle/renderer/VertexBuffer.h"
#include "eagle/renderer/VertexLayout.h"
#include "VulkanBuffer.h"
#include "VulkanCleaner.h"

namespace eagle {

struct VulkanVertexBufferCreateInfo {
    VulkanVertexBufferCreateInfo(const VertexLayout& layout):vertexLayout(layout) {}

    void* data;
    uint32_t size;
    VertexLayout vertexLayout;
    uint32_t bufferCount;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanVertexBuffer : public VertexBuffer, public VulkanCleanable {

public:

    VulkanVertexBuffer(VkDevice device, VulkanVertexBufferCreateInfo &createInfo, UpdateType usageFlags);
    virtual ~VulkanVertexBuffer();


    virtual uint32_t get_vertices_count() override;
    virtual uint32_t get_stride() override;

    virtual bool is_dirty() const override;
    virtual void flush(uint32_t bufferIndex) override;
    virtual void upload(void* data, uint32_t size) override;

    inline VulkanBuffer& get_buffer(uint32_t bufferIndex) {
        if (m_usage == UpdateType::CONSTANT){
            return *(m_buffers[0]);
        }
        return *(m_buffers[bufferIndex]);
    }

private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    VertexLayout m_layout;
    uint32_t m_size;
    UpdateType m_usage;
    char* m_data =  nullptr;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;


    bool m_resizing = false, m_initialized = false;

};

}

#endif //EAGLE_VULKANVERTEXBUFFER_H
