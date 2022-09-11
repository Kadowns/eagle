//
// Created by Novak on 01/04/2020.
//

#ifndef EAGLE_VULKANSTORAGEBUFFER_H
#define EAGLE_VULKANSTORAGEBUFFER_H

#include <eagle/renderer/storage_buffer.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_cleaner.h>

namespace eagle {

class VulkanSharedCommandPool;

struct VulkanStorageBufferCreateInfo{
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    size_t frameCount;
    VulkanCommandQueue* queue;

};

class VulkanStorageBuffer : public StorageBuffer, public VulkanCleanable {
public:
    explicit VulkanStorageBuffer(VulkanStorageBufferCreateInfo createInfo, size_t size, void* data, UpdateType usage);
    ~VulkanStorageBuffer() override;
    void copy_from(void *data, size_t size, size_t offset) override;
    void upload() override;

    DescriptorType type() const override;

    bool is_dirty() const override;
    void flush(uint32_t index) override;

    void cleanup();
    void recreate(uint32_t bufferCount);
    void create_storage_buffer();

    inline std::vector<std::shared_ptr<VulkanBuffer>>& get_buffers() { return m_buffers; }
private:
    VulkanStorageBufferCreateInfo m_createInfo;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
    bool m_cleared = true;
    bool m_dirtyBytes = false;

};

}

#endif //EAGLE_VULKANSTORAGEBUFFER_H
