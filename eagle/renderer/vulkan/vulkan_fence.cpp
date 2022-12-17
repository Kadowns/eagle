//
// Created by Ricardo on 14/08/2022.
//

#include <eagle/renderer/vulkan/vulkan_fence.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanFence::VulkanFence(const VulkanFenceCreateInfo& createInfo) : m_createInfo(createInfo), m_currentFence(VK_NULL_HANDLE) {

}

VulkanFence::~VulkanFence() {

}

bool VulkanFence::wait(std::chrono::milliseconds timeout) {

    if (!m_currentFence){
        return true;
    }

    auto result = vkWaitForFences(m_createInfo.device, 1, &m_currentFence, VK_TRUE, timeout.count());
    if (result == VK_SUCCESS){
        return true;
    }
    if (result == VK_TIMEOUT){
        return false;
    }
    throw VulkanException("failed to wait for fences", result);
}

void VulkanFence::acquire(VkFence fence) {
    m_currentFence = fence;
}

VkFence VulkanFence::current_fence() {
    return m_currentFence;
}

}
