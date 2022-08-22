//
// Created by Ricardo on 26/06/2022.
//

#include <eagle/renderer/vulkan/vulkan_gpu_queue.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_semaphore.h>
#include <eagle/renderer/vulkan/vulkan_fence.h>
#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>

namespace eagle {

namespace detail {

std::vector<VkSemaphore> getSemaphores(const std::span<Semaphore*>& semaphores, uint32_t frameIndex) {
    std::vector<VkSemaphore> result;
    result.reserve(semaphores.size());

    for (auto semaphore : semaphores) {
        auto castedSemaphore = dynamic_cast<VulkanSemaphore*>(semaphore);
        result.push_back(castedSemaphore->native_semaphore(frameIndex));
    }

    return result;
}

std::vector<VkCommandBuffer> getCommandBuffers(const std::span<CommandBuffer*>& commandBuffers, uint32_t frameIndex) {
    std::vector<VkCommandBuffer> result;
    result.reserve(commandBuffers.size());

    for (auto commandBuffer : commandBuffers) {
        auto castedCommandBuffer = dynamic_cast<VulkanCommandBuffer*>(commandBuffer);
        result.push_back(castedCommandBuffer->native_command_buffer(frameIndex));
    }

    return result;
}

std::vector<VkPipelineStageFlags> getPipelineStages(const std::span<PipelineStageFlags>& pipelineStages){
    std::vector<VkPipelineStageFlags> result;
    result.reserve(pipelineStages.size());

    for (auto stageMask : pipelineStages){
        result.push_back(VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(stageMask));
    }

    return result;
}

} // namespace detail

VulkanGPUQueue::VulkanGPUQueue(const GPUQueueCreateInfo &createInfo, const VulkanGPUQueueCreateInfo &nativeCreateInfo) :
        GPUQueue(createInfo),
        m_nativeCreateInfo(nativeCreateInfo) {



}

void VulkanGPUQueue::submit(const GPUQueueSubmitInfo &queueSubmitInfo) {

    auto currentFrame = *m_nativeCreateInfo.currentFrame;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    auto waitSemaphores = detail::getSemaphores(queueSubmitInfo.waitSemaphores, currentFrame);
    auto waitStages = detail::getPipelineStages(queueSubmitInfo.waitStages);
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    auto commandBuffers = detail::getCommandBuffers(queueSubmitInfo.commandBuffers, currentFrame);
    submitInfo.commandBufferCount = commandBuffers.size();
    submitInfo.pCommandBuffers = commandBuffers.data();

    auto signalSemaphores = detail::getSemaphores(queueSubmitInfo.signalSemaphores, currentFrame);
    submitInfo.signalSemaphoreCount = signalSemaphores.size();
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    auto castedFence = dynamic_cast<VulkanFence*>(queueSubmitInfo.fence);
    auto vkFence = castedFence->native_fence(currentFrame);

    VK_CALL vkResetFences(m_nativeCreateInfo.device, 1, &vkFence);

    VkResult result = vkQueueSubmit(m_queue, 1, &submitInfo, vkFence);

    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to submit command buffer!");
    }
}

} // eagle