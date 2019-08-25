//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include "eagle/core/renderer/UniformBuffer.h"
#include "VulkanBuffer.h"

EG_BEGIN

struct VulkanUniformBufferCreateInfo{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data);
    virtual ~VulkanUniformBuffer();

    virtual void flush(uint32_t bufferIndex) override;
    inline std::vector<Reference<VulkanBuffer>>& get_buffers() { return m_buffers; }

    void create_uniform_buffer();
    void cleanup();

    void upload_data(void* data);
    bool is_dirty();

private:

    VulkanUniformBufferCreateInfo m_info;
    std::vector<Reference<VulkanBuffer>> m_buffers;
    std::vector<uint32_t> m_dirtyBuffers;
    void* m_data = nullptr;
    bool m_cleared = true;

};

EG_END

#endif //EAGLE_VULKANUNIFORMBUFFER_H
