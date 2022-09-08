//
// Created by Ricardo on 03/09/2022.
//

#include <eagle/renderer/vulkan/vulkan_queue.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanQueue::ThreadCommandPool::ThreadCommandPool(const VulkanQueue::ThreadCommandPoolCreateInfo& createInfo) :
    m_createInfo(createInfo),
    m_commandPool(VK_NULL_HANDLE) {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = m_createInfo.queueFamilyIndex;

    auto result = vkCreateCommandPool(m_createInfo.device, &commandPoolCreateInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create a command pool", result);
    }
}

VulkanQueue::ThreadCommandPool::~ThreadCommandPool() {
    if (m_freeList.size() < m_allocations.size()){
        throw std::logic_error("tried to destroy a command pool that has allocated command buffers");
    }

    //if we get here, it means that every allocation is currently on the free list
    clear_cache();
    vkDestroyCommandPool(m_createInfo.device, m_commandPool, nullptr);
}

VkCommandBuffer VulkanQueue::ThreadCommandPool::allocate(VkCommandBufferLevel level) {
    for (auto i = 0; i < m_freeList.size(); i++) {

        auto freeCommandBuffer = m_freeList[i];
        auto freeAllocateInfo = m_allocations.at(freeCommandBuffer);

        if (freeAllocateInfo.level == level) {
            m_freeList.erase(m_freeList.begin() + i);
            return freeCommandBuffer;
        }
    }

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = m_commandPool;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.level = level;

    VkCommandBuffer commandBuffer;
    auto result = vkAllocateCommandBuffers(m_createInfo.device, &allocateInfo, &commandBuffer);

    if (result != VK_SUCCESS) {
        throw VulkanException("failed to allocate command buffers", result);
    }

    m_allocations.emplace(commandBuffer, allocateInfo);

    return commandBuffer;
}

void VulkanQueue::ThreadCommandPool::free(VkCommandBuffer& commandBuffer) {
    m_freeList.emplace_back(commandBuffer);
}

void VulkanQueue::ThreadCommandPool::clear_cache() {
    for (auto& freeCommandBuffer : m_freeList){
        m_allocations.erase(freeCommandBuffer);
    }
    vkFreeCommandBuffers(m_createInfo.device, m_commandPool, m_freeList.size(), m_freeList.data());
    m_freeList.clear();
}

VulkanQueue::VulkanQueue(const VulkanQueueCreateInfo& createInfo) :
        m_createInfo(createInfo) {
    vkGetDeviceQueue(m_createInfo.device, m_createInfo.familyIndex, m_createInfo.index, &m_queue);
}

void VulkanQueue::submit(const VkSubmitInfo& submitInfo, VkFence fence) {
    VkSubmitInfo submitInfos[] = {submitInfo};
    submit(submitInfos, fence);
}

void VulkanQueue::submit(std::span<VkSubmitInfo> submitInfo, VkFence fence) {
    auto result = vkQueueSubmit(m_queue, submitInfo.size(), submitInfo.data(), fence);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to submit to queue", result);
    }
}

VkQueue VulkanQueue::native_queue() const {
    return m_queue;
}

uint32_t VulkanQueue::family_index() const {
    return m_createInfo.familyIndex;
}

uint32_t VulkanQueue::index() const {
    return m_createInfo.index;
}

QueueType VulkanQueue::type() const {
    return m_createInfo.type;
}

void VulkanQueue::allocate(VkCommandBuffer& commandBuffer, VkCommandBufferLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto threadId = std::this_thread::get_id();

    auto it = m_threadCommandPools.find(threadId);
    if (it == m_threadCommandPools.end()){
        ThreadCommandPoolCreateInfo threadCommandPoolCreateInfo = {};
        threadCommandPoolCreateInfo.device = m_createInfo.device;
        threadCommandPoolCreateInfo.queueFamilyIndex = m_createInfo.familyIndex;
        it = m_threadCommandPools.emplace(threadId, std::make_unique<ThreadCommandPool>(threadCommandPoolCreateInfo)).first;
    }

    auto& threadCommandPools = it->second;
    commandBuffer = threadCommandPools->allocate(level);
}

void VulkanQueue::free(VkCommandBuffer& commandBuffer, const std::thread::id& threadId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_threadCommandPools.find(threadId);
    if (it == m_threadCommandPools.end()){
        throw std::logic_error("tried to free a command buffer on a thread that does not allocate a command pool");
    }

    auto& threadCommandPool = it->second;
    threadCommandPool->free(commandBuffer);
}

}