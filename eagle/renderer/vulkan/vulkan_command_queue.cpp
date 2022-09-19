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

static std::vector<VkCommandBuffer> native_command_buffers(const std::span<CommandBuffer*>& commandBuffers, uint32_t frameIndex) {
    std::vector<VkCommandBuffer> result;
    result.reserve(commandBuffers.size());

    for (auto commandBuffer : commandBuffers) {
        auto castedCommandBuffer = (VulkanCommandBuffer*)commandBuffer;
        result.push_back(castedCommandBuffer->native_command_buffer(frameIndex));
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
}

void VulkanCommandQueue::ThreadCommandPool::clear_cache() {
    for (auto& freeCommandBuffer : m_freeList){
        m_allocations.erase(freeCommandBuffer);
    }
    vkFreeCommandBuffers(m_createInfo.device, m_commandPool, m_freeList.size(), m_freeList.data());
    m_freeList.clear();
}

VulkanCommandQueue::VulkanCommandQueue(const VulkanQueueCreateInfo& createInfo) :
        m_createInfo(createInfo) {
    vkGetDeviceQueue(m_createInfo.device, m_createInfo.familyIndex, m_createInfo.index, &m_queue);
}

void VulkanCommandQueue::submit(std::span<CommandBufferSubmitInfo> submitInfos, Fence* fence) {

    std::vector<VkSubmitInfo> nativeSubmitInfos;
    nativeSubmitInfos.reserve(submitInfos.size());

    // We need this so we can keep our vectors alive outside our for loop
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

        submitInfoContent.commandBuffers = detail::native_command_buffers(submitInfo.commandBuffers, currentFrame);
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

    VkFence nativeFence = VK_NULL_HANDLE;

    if (fence) {
        auto castedFence = (VulkanFence*)fence;
        nativeFence = castedFence->native_fence(currentFrame);
    }

    submit(nativeSubmitInfos, nativeFence);
}

void VulkanCommandQueue::submit(const VkSubmitInfo& submitInfo, VkFence fence) {
    VkSubmitInfo submitInfos[] = {submitInfo};
    submit(submitInfos, fence);
}

void VulkanCommandQueue::submit(std::span<VkSubmitInfo> submitInfo, VkFence fence) {

    auto result = vkQueueSubmit(m_queue, submitInfo.size(), submitInfo.data(), fence);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to submit to queue", result);
    }
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

    auto it = m_threadCommandPools.find(threadId);
    if (it == m_threadCommandPools.end()){
        throw std::logic_error("tried to free a command buffer on a thread that does not allocate a command pool");
    }

    auto& threadCommandPool = it->second;
    threadCommandPool->free(commandBuffer);
}

}