//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_VULKANINDEXBUFFER_H
#define EAGLE_VULKANINDEXBUFFER_H

#include <set>

#include "eagle/core/renderer/IndexBuffer.h"
#include "VulkanBuffer.h"


EG_BEGIN

struct VulkanIndexBufferCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t bufferCount;
};

class VulkanIndexBuffer : public IndexBuffer {

public:

    VulkanIndexBuffer(VkDevice device, VulkanIndexBufferCreateInfo &createInfo, void *indexData,
                      size_t indexCount, INDEX_BUFFER_TYPE indexType, EG_BUFFER_USAGE usage);
    virtual ~VulkanIndexBuffer();
    virtual void * get_data() override;
    virtual size_t get_indices_count() override;

    bool is_dirty() const;
    void flush(uint32_t bufferIndex);
    void upload(void* data, uint32_t verticesCount);
    inline VulkanBuffer& get_buffer(uint32_t bufferIndex) {
        if (m_usage == EG_BUFFER_USAGE::CONSTANT){
            return *(m_buffers[0]);
        }
        return *(m_buffers[bufferIndex]);
    }

    inline VkIndexType get_native_index_type(){
        switch(m_indexType) {
            case INDEX_BUFFER_TYPE::UINT_16: return VK_INDEX_TYPE_UINT16;
            case INDEX_BUFFER_TYPE::UINT_32: return VK_INDEX_TYPE_UINT32;
        }
    }

private:
    VkDevice m_device;
    VkPhysicalDevice m_physicalDevice;
    std::vector<Reference<VulkanBuffer>> m_buffers;
    std::set<int> m_dirtyBuffers;
    char* m_data = nullptr;
    size_t m_indexCount;
    INDEX_BUFFER_TYPE m_indexType;
    EG_BUFFER_USAGE m_usage;
    bool m_resizing = false, m_initialized = false;
};

EG_END


#endif //EAGLE_VULKANINDEXBUFFER_H
