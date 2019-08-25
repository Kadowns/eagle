//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXBUFFER_H
#define EAGLE_VULKANVERTEXBUFFER_H

#include <set>

#include "eagle/core/renderer/VertexBuffer.h"
#include "eagle/core/renderer/VertexLayout.h"
#include "VulkanBuffer.h"


EG_BEGIN

struct VulkanVertexBufferCreateInfo {
    VulkanVertexBufferCreateInfo(const VertexLayout& layout):vertexLayout(layout) {}

    void* data;
    uint32_t count;
    VertexLayout vertexLayout;
    uint32_t bufferCount;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanVertexBuffer : public VertexBuffer {

public:

    VulkanVertexBuffer(VkDevice device, VulkanVertexBufferCreateInfo &createInfo, EG_BUFFER_USAGE usageFlags);
    virtual ~VulkanVertexBuffer();


    virtual uint32_t get_vertices_count() override;
    virtual uint32_t get_stride() override;

    bool is_dirty() const;
    void flush(uint32_t bufferIndex);
    void upload(void* data, uint32_t elementCount);
    inline VulkanBuffer& get_buffer(uint32_t bufferIndex) {
        if (m_usage == EG_BUFFER_USAGE::CONSTANT){
            return *(m_buffers[0]);
        }
        return *(m_buffers[bufferIndex]);
    }

private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    VertexLayout m_layout;
    uint32_t m_elementCount;
    EG_BUFFER_USAGE m_usage;
    char* m_data =  nullptr;

    std::vector<Reference<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;


    bool m_resizing = false, m_initialized = false;

};

EG_END

#endif //EAGLE_VULKANVERTEXBUFFER_H
