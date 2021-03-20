//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include "eagle/renderer/uniform_buffer.h"
#include "vulkan_buffer.h"
#include "vulkan_cleaner.h"


namespace eagle {

struct VulkanUniformBufferCreateInfo{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer, public VulkanCleanable {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data);
    virtual ~VulkanUniformBuffer();

    inline std::vector<std::shared_ptr<VulkanBuffer>>& get_buffers() { return m_buffers; }
    void create_uniform_buffer();

    DescriptorType type() const override;

    void cleanup();
    void recreate(uint32_t bufferCount);
    virtual void flush(uint32_t bufferIndex) override;
    virtual void set_data(void *data, size_t size, size_t offset) override;
    virtual void push() override;
    virtual bool is_dirty() const override;

private:

    VulkanUniformBufferCreateInfo m_info;
    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
    bool m_cleared = true;
    bool m_dirtyBytes = false;

};

}

#endif //EAGLE_VULKANUNIFORMBUFFER_H
