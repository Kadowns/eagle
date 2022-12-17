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

class VulkanFence;

struct VulkanQueueCreateInfo {
    VkDevice device = VK_NULL_HANDLE;
    uint32_t familyIndex = 0;
    uint32_t index = 0;
    uint32_t* currentFrame = nullptr;
    CommandQueueType type = CommandQueueType::UNDEFINED;
};

class VulkanCommandQueue : public CommandQueue {
private:

    class FencePool {
    public:

        FencePool(VkDevice device);

        ~FencePool();

        VkFence allocate();

        void free(VkFence fence);

    private:
        VkDevice m_device;
        std::vector<VkFence> m_fences;
    };

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

        void mark_as_pending(VkCommandBuffer& commandBuffer);

        void clear_cache();

    private:
        ThreadCommandPoolCreateInfo m_createInfo;
        VkCommandPool m_commandPool;
        std::unordered_map<VkCommandBuffer, VkCommandBufferAllocateInfo> m_allocations;
        std::vector<VkCommandBuffer> m_freeList;
        std::set<VkCommandBuffer> m_pendingCommandBuffers;
    };
public:
    explicit VulkanCommandQueue(const VulkanQueueCreateInfo& createInfo);

    void submit(std::span<CommandBufferSubmitInfo> submitInfos, Fence* fence) override;

    void idle() override;

    void submit(std::span<VkSubmitInfo> submitInfos, VulkanFence* fence);

    void submit(const VkSubmitInfo& submitInfo, VulkanFence* fence);

    VkQueue native_queue() const;

    uint32_t family_index() const;

    uint32_t index() const;

    CommandQueueType type() const;

    void allocate(VkCommandBuffer& commandBuffer, VkCommandBufferLevel level);

    void free(VkCommandBuffer& commandBuffer, const std::thread::id& threadId = std::this_thread::get_id());
private:

    void check_pending_submissions();

private:
    VulkanQueueCreateInfo m_createInfo;
    VkQueue m_queue = VK_NULL_HANDLE;
    FencePool m_fencePool;
    std::mutex m_mutex;
    std::unordered_map<std::thread::id, std::unique_ptr<ThreadCommandPool>> m_threadCommandPools;
    std::unordered_map<VkFence, std::set<VkCommandBuffer>> m_pendingSubmissions;
    std::unordered_map<VkFence, std::vector<std::pair<VkCommandBuffer, std::thread::id>>> m_commandBuffersToFree;
};
}

#endif //EAGLE_VULKAN_QUEUE_H
