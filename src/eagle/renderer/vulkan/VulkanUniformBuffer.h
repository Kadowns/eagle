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
    ShaderItemLayout layout;
    VkDescriptorSetLayoutBinding layoutBindings;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo,
                                 UNIFORM_BUFFER_USAGE usageFlags);
    virtual ~VulkanUniformBuffer();

    virtual void flush(void *data, uint32_t bufferIndex) override;
    virtual void bind() override;
private:

    std::vector<VulkanBuffer> m_buffers;
    VulkanUniformBufferCreateInfo m_info;
};

_EAGLE_END

#endif //EAGLE_VULKANUNIFORMBUFFER_H
