//
// Created by Ricardo on 26/06/2022.
//

#include <eagle/renderer/vulkan/vulkan_semaphore.h>


namespace eagle {

VulkanSemaphore::VulkanSemaphore(const VulkanSemaphoreCreateInfo& createInfo) : m_createInfo(createInfo) {
    m_semaphores.resize(m_createInfo.frameCount);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for (auto& semaphore : m_semaphores){
        vkCreateSemaphore(m_createInfo.device, &semaphoreCreateInfo, nullptr, &semaphore);
    }
}
VulkanSemaphore::~VulkanSemaphore() {
    for (auto& semaphore : m_semaphores){
        vkDestroySemaphore(m_createInfo.device, semaphore, nullptr);
    }
}

VkSemaphore VulkanSemaphore::native_semaphore(uint32_t frame) {
    return m_semaphores[frame];
}


} // eagle