//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include "eagle/renderer/UniformBuffer.h"
#include "eagle/renderer/vulkan/VulkanBuffer.h"

_EAGLE_BEGIN

struct VulkanUniformBufferCreateInfo{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, const ShaderItemLayout& layout);
    virtual ~VulkanUniformBuffer();

    virtual void flush(void *data, uint32_t bufferIndex) override;
    virtual size_t size() override;
    inline std::vector<std::shared_ptr<VulkanBuffer>>& get_buffers() { return m_buffers; }
    void create_uniform_buffer();
    void cleanup();

private:

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    VulkanUniformBufferCreateInfo m_info;
    bool m_cleared = true;
};

_EAGLE_END

#endif //EAGLE_VULKANUNIFORMBUFFER_H
