//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_VULKANVERTEXBUFFER_H
#define EAGLE_VULKANVERTEXBUFFER_H

#include <memory>

#include "eagle/renderer/VertexBuffer.h"
#include "VulkanBuffer.h"


_EAGLE_BEGIN

struct VulkanVertexBufferCreateInfo {
    std::vector<float> vertices;
    uint32_t stride;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanVertexBuffer : public VertexBuffer {

public:

    VulkanVertexBuffer(VkDevice device, VulkanVertexBufferCreateInfo &createInfo);
    virtual ~VulkanVertexBuffer();
    virtual const std::vector<float>& get_vertices() override;
    virtual const void* get_vertices_data() override;
    virtual uint32_t get_vertices_count() override;
    virtual uint32_t get_stride() override;

    inline VulkanBuffer& get_buffer() { return *m_buffer; }

private:
    VkDevice m_device;
    std::vector<float> m_vertices;
    uint32_t m_stride;

    std::shared_ptr<VulkanBuffer> m_buffer;

};

_EAGLE_END

#endif //EAGLE_VULKANVERTEXBUFFER_H
