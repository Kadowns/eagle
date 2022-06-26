//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_VULKANINDEXBUFFER_H
#define EAGLE_VULKANINDEXBUFFER_H

#include <eagle/renderer/index_buffer.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_cleaner.h>
#include <eagle/renderer/vulkan/vulkan_deleter.h>

#include <set>

namespace eagle {

struct VulkanIndexBufferCreateInfo {
    VulkanDeleter& deleter;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t bufferCount;
};

class VulkanIndexBuffer : public IndexBuffer, public VulkanCleanable {

public:

    VulkanIndexBuffer(const IndexBufferCreateInfo& createInfo, const VulkanIndexBufferCreateInfo &vulkanCreateInfo, void* data, size_t size);
    ~VulkanIndexBuffer() override;

    bool is_dirty() const override;
    void flush(uint32_t bufferIndex) override;

    void flush() override;
    inline VulkanBuffer& native_buffer(uint32_t bufferIndex) {
        return *(m_buffers[bufferIndex]);
    }

    inline VkIndexType native_index_type(){
        switch(m_info.indexType) {
            case IndexBufferType::UINT_16: return VK_INDEX_TYPE_UINT16;
            case IndexBufferType::UINT_32: return VK_INDEX_TYPE_UINT32;
        }
    }

private:
    VulkanIndexBufferCreateInfo m_vulkanCreateInfo;

    std::vector<std::shared_ptr<VulkanBuffer>> m_buffers;
    std::set<uint32_t> m_dirtyBuffers;

};

}


#endif //EAGLE_VULKANINDEXBUFFER_H
