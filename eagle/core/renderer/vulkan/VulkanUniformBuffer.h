//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include "eagle/core/renderer/UniformBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanCleaner.h"


EG_BEGIN

struct VulkanUniformBufferCreateInfo{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t bufferCount;
};

class VulkanUniformBuffer : public UniformBuffer, public VulkanCleanable {

public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data);
    virtual ~VulkanUniformBuffer();

    inline std::vector<Reference<VulkanBuffer>>& get_buffers() { return m_buffers; }
    void create_uniform_buffer();

    void cleanup();
    virtual void flush(uint32_t bufferIndex) override;
    virtual void set_data(void *data, size_t size, size_t offset) override;
    virtual void push() override;
    virtual bool is_dirty() const override;

private:

    VulkanUniformBufferCreateInfo m_info;
    std::vector<Reference<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
    bool m_cleared = true;
    bool m_dirtyBytes = false;

};

EG_END

#endif //EAGLE_VULKANUNIFORMBUFFER_H
