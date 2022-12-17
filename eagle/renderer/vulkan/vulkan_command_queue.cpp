//
// Created by Ricardo on 03/09/2022.
//

#include <eagle/renderer/vulkan/vulkan_command_queue.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>
#include <eagle/renderer/vulkan/vulkan_semaphore.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_fence.h>

namespace eagle {

namespace detail {

static std::vector<VkSemaphore> native_semaphores(const std::span<Semaphore*> semaphores, size_t frameIndex){
    std::vector<VkSemaphore> nativeSemaphores;
    nativeSemaphores.reserve(semaphores.size());
    for (auto semaphore : semaphores){
        auto castedSemaphore = (VulkanSemaphore*)semaphore;
        nativeSemaphores.emplace_back(castedSemaphore->native_semaphore(frameIndex));
    }
    return nativeSemaphores;
}

static std::vector<VkCommandBuffer> native_command_buffers(const std::span<CommandBuffer*>& commandBuffers) {
    std::vector<VkCommandBuffer> result;
    result.reserve(commandBuffers.size());

    for (auto commandBuffer : commandBuffers) {
        auto castedCommandBuffer = (VulkanCommandBuffer*)commandBuffer;
        result.push_back(castedCommandBuffer->current_command_buffer());
    }

    return result;
}

static std::vector<VkPipelineStageFlags> native_pipeline_stages(const std::span<PipelineStageFlags>& pipelineStages){
    std::vector<VkPipelineStageFlags> result;
    result.reserve(pipelineStages.size());

    for (auto stageMask : pipelineStages){
        result.push_back(VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(stageMask));
    }

    return result;
}
}

VulkanCommandQueue::FencePool::FencePool(VkDevice device) : m_device(device) {

}

VulkanCommandQueue::FencePool::~FencePool() {
    for (auto fence : m_fences){
        vkDestroyFence(m_device, fence, nullptr);
    }
}

VkFence VulkanCommandQueue::FencePool::allocate() {
    VkFence fence = VK_NULL_HANDLE;
    if (m_fences.empty()){

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        auto result = vkCreateFence(m_device, &fenceCreateInfo, nullptr, &fence);
        if (result != VK_SUCCESS) {
            throw VulkanException("failed to create a fence", result);
        }

        return fence;
    }

    fence = m_fences.back();
    m_fences.pop_back();
    return fence;
}

void VulkanCommandQueue::FencePool::free(VkFence fence) {
    m_fences.emplace_back(fence);
}

VulkanCommandQueue::ThreadCommandPool::ThreadCommandPool(const VulkanCommandQueue::ThreadCommandPoolCreateInfo& createInfo) :
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

VulkanCommandQueue::ThreadCommandPool::~ThreadCommandPool() {
    if (m_freeList.size() < m_allocations.size()){
        throw std::logic_error("tried to destroy a command pool that has allocated command buffers");
    }

    //if we get here, it means that every allocation is currently on the free list
    clear_cache();
    vkDestroyCommandPool(m_createInfo.device, m_commandPool, nullptr);
}

VkCommandBuffer VulkanCommandQueue::ThreadCommandPool::allocate(VkCommandBufferLevel level) {
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

void VulkanCommandQueue::ThreadCommandPool::free(VkCommandBuffer& commandBuffer) {
    m_freeList.emplace_back(commandBuffer);
    commandBuffer = VK_NULL_HANDLE;
}

void VulkanCommandQueue::ThreadCommandPool::mark_as_pending(VkCommandBuffer& commandBuffer) {
    m_pendingCommandBuffers.insert(commandBuffer);
}

void VulkanCommandQueue::ThreadCommandPool::clear_cache() {
    for (auto& freeCommandBuffer : m_freeList){
        m_allocations.erase(freeCommandBuffer);
    }
    vkFreeCommandBuffers(m_createInfo.device, m_commandPool, m_freeList.size(), m_freeList.data());
    m_freeList.clear();
}

VulkanCommandQueue::VulkanCommandQueue(const VulkanQueueCreateInfo& createInfo) :
        m_createInfo(createInfo), m_fencePool(createInfo.device) {
    vkGetDeviceQueue(m_createInfo.device, m_createInfo.familyIndex, m_createInfo.index, &m_queue);
}

void VulkanCommandQueue::submit(std::span<CommandBufferSubmitInfo> submitInfos, Fence* fence) {

    std::vector<VkSubmitInfo> nativeSubmitInfos;
    nativeSubmitInfos.reserve(submitInfos.size());

    // We need this so we can keep our vectors alive outside our conversion loop
    struct SubmitInfoContent {
        std::vector<VkSemaphore> waitSemaphores;
        std::vector<VkPipelineStageFlags> waitStages;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkSemaphore> signalSemaphores;
    };
    std::vector<SubmitInfoContent> submitInfoContents;
    submitInfoContents.reserve(submitInfos.size());


    auto currentFrame = *m_createInfo.currentFrame;

    for (auto& submitInfo : submitInfos){
        SubmitInfoContent submitInfoContent = {};
        VkSubmitInfo nativeSubmitInfo = {};
        nativeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        submitInfoContent.waitSemaphores = detail::native_semaphores(submitInfo.waitSemaphores, currentFrame);
        submitInfoContent.waitStages = detail::native_pipeline_stages(submitInfo.waitStages);

        nativeSubmitInfo.waitSemaphoreCount = submitInfoContent.waitSemaphores.size();
        nativeSubmitInfo.pWaitSemaphores = submitInfoContent.waitSemaphores.data();
        nativeSubmitInfo.pWaitDstStageMask = submitInfoContent.waitStages.data();

        submitInfoContent.commandBuffers = detail::native_command_buffers(submitInfo.commandBuffers);
        nativeSubmitInfo.commandBufferCount = submitInfoContent.commandBuffers.size();
        nativeSubmitInfo.pCommandBuffers = submitInfoContent.commandBuffers.data();

        submitInfoContent.signalSemaphores = detail::native_semaphores(submitInfo.signalSemaphores, currentFrame);
        nativeSubmitInfo.signalSemaphoreCount = submitInfoContent.signalSemaphores.size();
        nativeSubmitInfo.pSignalSemaphores = submitInfoContent.signalSemaphores.data();

        //if we do not keep them alive in the content vector
        //all our native vectors will be destroyed as soon as we move out
        //of this loop scope
        submitInfoContents.emplace_back(std::move(submitInfoContent));

        nativeSubmitInfos.emplace_back(nativeSubmitInfo);
    }

    submit(nativeSubmitInfos, (VulkanFence*)fence);
}

void VulkanCommandQueue::idle() {
    vkQueueWaitIdle(m_queue);
}

void VulkanCommandQueue::submit(const VkSubmitInfo& submitInfo, VulkanFence* fence) {
    VkSubmitInfo submitInfos[] = {submitInfo};
    submit(submitInfos, fence);
}

void VulkanCommandQueue::submit(std::span<VkSubmitInfo> submitInfos, VulkanFence* fence) {

    auto nativeFence = m_fencePool.allocate();

    vkResetFences(m_createInfo.device, 1, &nativeFence);

    if (fence){
        fence->acquire(nativeFence);
    }

    auto result = vkQueueSubmit(m_queue, submitInfos.size(), submitInfos.data(), nativeFence);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to submit to queue", result);
    }

    std::set<VkCommandBuffer> commandBuffers;
    for (auto& submitInfo : submitInfos) {
        commandBuffers.insert(submitInfo.pCommandBuffers, submitInfo.pCommandBuffers + submitInfo.commandBufferCount);
    }
    m_pendingSubmissions.emplace(nativeFence, std::move(commandBuffers));
}

VkQueue VulkanCommandQueue::native_queue() const {
    return m_queue;
}

uint32_t VulkanCommandQueue::family_index() const {
    return m_createInfo.familyIndex;
}

uint32_t VulkanCommandQueue::index() const {
    return m_createInfo.index;
}

CommandQueueType VulkanCommandQueue::type() const {
    return m_createInfo.type;
}

void VulkanCommandQueue::allocate(VkCommandBuffer& commandBuffer, VkCommandBufferLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);

    check_pending_submissions();

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

void VulkanCommandQueue::free(VkCommandBuffer& commandBuffer, const std::thread::id& threadId) {
    std::lock_guard<std::mutex> lock(m_mutex);

    check_pending_submissions();

    auto it = m_threadCommandPools.find(threadId);
    if (it == m_threadCommandPools.end()){
        throw std::logic_error("tried to free a command buffer on a thread that does not allocate a command pool");
    }

    //check if it is in pending state
    for (auto& [fence, commandBuffers] : m_pendingSubmissions){

        if (commandBuffers.find(commandBuffer) == commandBuffers.end()){
            //it was not present in this submission, try the next one
            continue;
        }

        //it IS in pending state, only free it when its submission actually ends
        m_commandBuffersToFree[fence].emplace_back(std::make_pair(commandBuffer, threadId));
        return;
    }

    auto& threadCommandPool = it->second;
    threadCommandPool->free(commandBuffer);
}

void VulkanCommandQueue::check_pending_submissions() {
    auto pendingSubmissions = m_pendingSubmissions;
    for (auto& [fence, commandBuffers] : pendingSubmissions) {

        auto result = vkWaitForFences(m_createInfo.device, 1, &fence, VK_TRUE, 0);

        if (result == VK_SUCCESS){

            //we will only free command buffers which were actually freed during the time that they were in pending state
            auto& commandBuffersToFree = m_commandBuffersToFree[fence];
            for (auto& [commandBuffer, threadId] : commandBuffersToFree){
                m_threadCommandPools.at(threadId)->free(commandBuffer);
            }

            m_fencePool.free(fence);
            m_pendingSubmissions.erase(fence);
            m_commandBuffersToFree.erase(fence);
            continue;
        }
        if (result == VK_TIMEOUT){
            continue;
        }
        throw VulkanException("failed to wait for fences", result);
    }

}


}