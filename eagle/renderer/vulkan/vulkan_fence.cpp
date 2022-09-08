//
// Created by Ricardo on 14/08/2022.
//

#include <eagle/renderer/vulkan/vulkan_fence.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanFence::VulkanFence(const VulkanFenceCreateInfo& createInfo) : m_createInfo(createInfo) {

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    m_fences.resize(m_createInfo.frameCount);
    for (auto& fence : m_fences){
        vkCreateFence(m_createInfo.device, &fenceCreateInfo, nullptr, &fence);
    }
}

VulkanFence::~VulkanFence() {
    for (auto& fence : m_fences){
        vkDestroyFence(m_createInfo.device, fence, nullptr);
    }
}

bool VulkanFence::wait(std::chrono::milliseconds timeout) {

    auto result = vkWaitForFences(m_createInfo.device, 1, &m_fences[*m_createInfo.currentFrame], VK_TRUE, timeout.count());
    if (result == VK_SUCCESS){
        return true;
    }
    if (result == VK_TIMEOUT){
        return false;
    }
    throw VulkanException("failed to wait for fences", result);
}

void VulkanFence::reset() {
    auto result = vkResetFences(m_createInfo.device, 1, &m_fences[*m_createInfo.currentFrame]);
    if (result != VK_SUCCESS){
        throw VulkanException("failed to reset fences", result);
    }
}

VkFence VulkanFence::native_fence(size_t index) {
    return m_fences[index];
}

}
