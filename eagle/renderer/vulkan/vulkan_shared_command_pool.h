//
// Created by Ricardo on 03/09/2022.
//

#define EAGLE_VULKAN_SHARED_COMMAND_POOL_H
#ifndef EAGLE_VULKAN_SHARED_COMMAND_POOL_H
#define EAGLE_VULKAN_SHARED_COMMAND_POOL_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_queue.h>

#include <thread>
#include <mutex>
#include <span>
#include <unordered_map>

namespace eagle {

struct VulkanSharedCommandPoolCreateInfo {
    const std::unordered_map<QueueType, VulkanQueue>& queues;
    VkDevice device;
};

/// A command pool for allocating command buffers on multiple threads
class VulkanSharedCommandPool {
private:

    class QueueCommandPool {
    public:
        QueueCommandPool(VkDevice device, const VulkanQueue& queue);
        ~QueueCommandPool();

        VkCommandBuffer allocate(VkCommandBufferLevel level);

        void free(VkCommandBuffer commandBuffer);

        void clear_cache();

    private:
        VkDevice m_device;
        const VulkanQueue& m_queue;
        VkCommandPool m_commandPool;
        std::unordered_map<VkCommandBuffer, VkCommandBufferAllocateInfo> m_allocations;
        std::vector<VkCommandBuffer> m_freeList;
    };

    class ThreadCommandPools {
    public:
        explicit ThreadCommandPools(const VulkanSharedCommandPoolCreateInfo& createInfo, const std::thread::id& threadId);

        std::thread::id thread_id() const;

        VkCommandBuffer allocate(VkCommandBufferLevel level, QueueType queueType);

        void free(VkCommandBuffer commandBuffer, QueueType queueType);

    private:
        const VulkanSharedCommandPoolCreateInfo& m_createInfo;
        std::thread::id m_threadId;
        std::unordered_map<QueueType, QueueCommandPool> m_queueCommandPools;
    };

    using ThreadCommandPoolMap = std::unordered_map<std::thread::id, ThreadCommandPools>;
    using ThreadCommandPoolMapIterator = typename ThreadCommandPoolMap::iterator;
public:

    VulkanSharedCommandPool(const VulkanSharedCommandPoolCreateInfo& createInfo);
    ~VulkanSharedCommandPool();

    void allocate(const std::span<VkCommandBuffer>& commandBuffers, VkCommandBufferLevel level, QueueType queueType);

    void free(const std::span<VkCommandBuffer>& commandBuffers, QueueType queueType, const std::thread::id& threadId = std::this_thread::get_id());

private:

    ThreadCommandPoolMapIterator create_command_pool_for_thread(const std::thread::id& threadId);
    ThreadCommandPoolMapIterator find_command_pool_for_thread(const std::thread::id& threadId, bool createIfNotFound);

private:
    VulkanSharedCommandPoolCreateInfo m_createInfo;
    std::mutex m_mutex;
    ThreadCommandPoolMap m_threadCommandPools;
};

}

#endif //EAGLE_VULKAN_SHARED_COMMAND_POOL_H
