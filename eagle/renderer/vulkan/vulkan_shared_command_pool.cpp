//
// Created by Ricardo on 03/09/2022.
//

#include <eagle/renderer/vulkan/vulkan_shared_command_pool.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanSharedCommandPool::QueueCommandPool::QueueCommandPool(VkDevice device, const VulkanQueue& queue) :
    m_device(device),
    m_queue(queue),
    m_commandPool(VK_NULL_HANDLE) {
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolCreateInfo.queueFamilyIndex = queue.family_index();

    auto result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_commandPool);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create a command pool", result);
    }
}

VulkanSharedCommandPool::QueueCommandPool::~QueueCommandPool() {
    if (m_freeList.size() < m_allocations.size()){
        throw std::logic_error("tried to destroy a command pool that has allocated command buffers");
    }

    //if we get here, it means that every allocation is currently on the free list
    clear_cache();
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);
}

VkCommandBuffer VulkanSharedCommandPool::QueueCommandPool::allocate(VkCommandBufferLevel level) {

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
    auto result = vkAllocateCommandBuffers(m_device, &allocateInfo, &commandBuffer);

    if (result != VK_SUCCESS) {
        throw VulkanException("failed to allocate command buffers", result);
    }

    m_allocations.emplace(commandBuffer, allocateInfo);

    return commandBuffer;
}

void VulkanSharedCommandPool::QueueCommandPool::free(VkCommandBuffer commandBuffer) {
    m_freeList.emplace_back(commandBuffer);
}

void VulkanSharedCommandPool::QueueCommandPool::clear_cache() {
    for (auto& freeCommandBuffer : m_freeList){
        m_allocations.erase(freeCommandBuffer);
    }
    vkFreeCommandBuffers(m_device, m_commandPool, m_freeList.size(), m_freeList.data());
    m_freeList.clear();
}

VulkanSharedCommandPool::ThreadCommandPools::ThreadCommandPools(const VulkanSharedCommandPoolCreateInfo& createInfo, const std::thread::id& threadId) :
    m_createInfo(createInfo),
    m_threadId(threadId) {

}

std::thread::id VulkanSharedCommandPool::ThreadCommandPools::thread_id() const {
    return m_threadId;
}

VkCommandBuffer VulkanSharedCommandPool::ThreadCommandPools::allocate(VkCommandBufferLevel level, QueueType queueType) {

    auto queueCommandPool = m_queueCommandPools.find(queueType);
    if (queueCommandPool == m_queueCommandPools.end()){
        const auto& queue = m_createInfo.queues.at(queueType);
        queueCommandPool = m_queueCommandPools.emplace(queueType, QueueCommandPool(m_createInfo.device, queue)).first;
    }

    return queueCommandPool->second.allocate(level);
}

void VulkanSharedCommandPool::ThreadCommandPools::free(VkCommandBuffer commandBuffer, QueueType queueType) {
    auto queueCommandPool = m_queueCommandPools.find(queueType);
    if (queueCommandPool == m_queueCommandPools.end()){
        throw std::logic_error("tried to free a command buffer on a command pool that doesn't contain it's queue");
    }
    queueCommandPool->second.free(commandBuffer);
}

VulkanSharedCommandPool::VulkanSharedCommandPool(const VulkanSharedCommandPoolCreateInfo& createInfo) :
    m_createInfo(createInfo) {

}

VulkanSharedCommandPool::~VulkanSharedCommandPool() = default;

void VulkanSharedCommandPool::allocate(const std::span<VkCommandBuffer>& commandBuffers, VkCommandBufferLevel level, QueueType queueType) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = find_command_pool_for_thread(std::this_thread::get_id(), true);

    auto& threadCommandPools = it->second;

    for (auto& commandBuffer : commandBuffers){
        commandBuffer = threadCommandPools.allocate(level, queueType);
    }
}

void VulkanSharedCommandPool::free(const std::span<VkCommandBuffer>& commandBuffers, QueueType queueType, const std::thread::id& threadId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = find_command_pool_for_thread(threadId, false);
    if (it == m_threadCommandPools.end()){
        throw std::logic_error("tried to free a command buffer on a thread that does not allocate a command pool");
    }

    auto& threadCommandPools = it->second;

    for (auto& commandBuffer : commandBuffers){
        threadCommandPools.free(commandBuffer, queueType);
    }
}

VulkanSharedCommandPool::ThreadCommandPoolMapIterator VulkanSharedCommandPool::create_command_pool_for_thread(const std::thread::id& threadId) {
    return m_threadCommandPools.emplace(threadId, ThreadCommandPools(m_createInfo, threadId)).first;
}

VulkanSharedCommandPool::ThreadCommandPoolMapIterator VulkanSharedCommandPool::find_command_pool_for_thread(const std::thread::id& threadId, bool createIfNotFound) {
    auto it = m_threadCommandPools.find(threadId);
    if (it == m_threadCommandPools.end() && createIfNotFound){
        it = create_command_pool_for_thread(threadId);
    }
    return it;
}

}