//
// Created by Ricardo on 03/09/2022.
//

#ifndef EAGLE_VULKAN_QUEUE_H
#define EAGLE_VULKAN_QUEUE_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/command_queue.h>

#include <span>
#include <thread>

namespace eagle {

struct VulkanQueueCreateInfo {
    VkDevice device = VK_NULL_HANDLE;
    uint32_t familyIndex = 0;
    uint32_t index = 0;
    uint32_t* currentFrame = nullptr;
    CommandQueueType type = CommandQueueType::UNDEFINED;
};

class VulkanCommandQueue : public CommandQueue {
private:

    struct ThreadCommandPoolCreateInfo {
        VkDevice device;
        uint32_t queueFamilyIndex;
    };

    class ThreadCommandPool {
    public:
        explicit ThreadCommandPool(const ThreadCommandPoolCreateInfo& createInfo);
        ~ThreadCommandPool();

        VkCommandBuffer allocate(VkCommandBufferLevel level);

        void free(VkCommandBuffer& commandBuffer);

        void clear_cache();

    private:
        ThreadCommandPoolCreateInfo m_createInfo;
        VkCommandPool m_commandPool;
        std::unordered_map<VkCommandBuffer, VkCommandBufferAllocateInfo> m_allocations;
        std::vector<VkCommandBuffer> m_freeList;
    };
public:
    explicit VulkanCommandQueue(const VulkanQueueCreateInfo& createInfo);

    void submit(std::span<CommandBufferSubmitInfo> submitInfos, Fence* fence) override;

    void submit(std::span<VkSubmitInfo> submitInfos, VkFence fence);

    void submit(const VkSubmitInfo& submitInfo, VkFence fence);

    VkQueue native_queue() const;

    uint32_t family_index() const;

    uint32_t index() const;

    CommandQueueType type() const;

    void allocate(VkCommandBuffer& commandBuffer, VkCommandBufferLevel level);

    void free(VkCommandBuffer& commandBuffer, const std::thread::id& threadId = std::this_thread::get_id());

private:
    VulkanQueueCreateInfo m_createInfo;
    VkQueue m_queue = VK_NULL_HANDLE;
    std::mutex m_mutex;
    std::unordered_map<std::thread::id, std::unique_ptr<ThreadCommandPool>> m_threadCommandPools;
};
}

#endif //EAGLE_VULKAN_QUEUE_H
