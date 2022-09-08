//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_VULKANUNIFORMBUFFER_H
#define EAGLE_VULKANUNIFORMBUFFER_H

#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_cleaner.h>

namespace eagle {

struct VulkanUniformBufferCreateInfo {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t frameCount;
};

class VulkanUniformBuffer : public UniformBuffer, public VulkanCleanable {
public:

    explicit VulkanUniformBuffer(VulkanUniformBufferCreateInfo &createInfo, size_t size, void *data);
    ~VulkanUniformBuffer() override;

    inline std::vector<std::shared_ptr<VulkanBuffer>>& buffers() { return m_buffers; }
    void create_uniform_buffer();
    void cleanup();
    void recreate(uint32_t bufferCount);

    //uniform buffer
    void flush() override;

    //vulkan cleanable
    void flush(uint32_t bufferIndex) override;
    bool is_dirty() const override;

private:

    VulkanUniformBufferCreateInfo m_info;
    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<uint32_t> m_dirtyBuffers;
    bool m_cleared = true;
};

}

#endif //EAGLE_VULKANUNIFORMBUFFER_H
