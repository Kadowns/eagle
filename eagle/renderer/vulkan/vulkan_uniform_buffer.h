//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include "eagle/renderer/uniform_buffer.h"
#include "vulkan_buffer.h"
#include "vulkan_cleaner.h"
#include "vulkan_deleter.h"

namespace eagle {

struct VulkanUniformBufferCreateInfo{
    VulkanDeleter& deleter;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer, public VulkanCleanable {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data);
    ~VulkanUniformBuffer() override;

    inline std::vector<std::shared_ptr<VulkanBuffer>>& get_buffers() { return m_buffers; }
    void create_uniform_buffer();

    DescriptorType type() const override;

    void cleanup();
    void recreate(uint32_t bufferCount);

    //uniform buffer
    void copy_from(void *data, size_t size, size_t offset) override;
    void upload() override;

    //vulkan cleanable
    void flush(uint32_t bufferIndex) override;
    bool is_dirty() const override;

private:

    VulkanUniformBufferCreateInfo m_info;
    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
    bool m_cleared = true;
};

}

#endif //EAGLE_VULKANUNIFORMBUFFER_H
