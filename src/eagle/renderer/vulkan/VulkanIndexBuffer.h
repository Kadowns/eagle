//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_VULKANINDEXBUFFER_H
#define EAGLE_VULKANINDEXBUFFER_H

#include <memory>

#include "eagle/renderer/IndexBuffer.h"
#include "VulkanBuffer.h"


_EAGLE_BEGIN

struct VulkanIndexBufferCreateInfo {
    std::vector<uint32_t> indices;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanIndexBuffer : public IndexBuffer {

public:

    VulkanIndexBuffer(VkDevice device, VulkanIndexBufferCreateInfo &createInfo);
    virtual ~VulkanIndexBuffer();
    virtual const std::vector<uint32_t>& get_indices() override;
    virtual const void* get_indices_data() override;
    virtual uint32_t get_indices_count() override;

    inline VulkanBuffer& get_buffer() { return *m_buffer; }

private:
    VkDevice m_device;
    std::vector<uint32_t> m_indices;

    std::shared_ptr<VulkanBuffer> m_buffer;
};

_EAGLE_END


#endif //EAGLE_VULKANINDEXBUFFER_H
