//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_VULKAN_SEMAPHORE_H
#define EAGLE_VULKAN_SEMAPHORE_H

#include <eagle/renderer/semaphore.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

#include <vector>

namespace eagle {

struct VulkanSemaphoreCreateInfo {
    VkDevice device;
    uint32_t frameCount;
};

class VulkanSemaphore : public Semaphore {
public:
    explicit VulkanSemaphore(const VulkanSemaphoreCreateInfo& createInfo);
    ~VulkanSemaphore() override;

    VkSemaphore native_semaphore(uint32_t frame);

private:
    VulkanSemaphoreCreateInfo m_createInfo;
    std::vector<VkSemaphore> m_semaphores;
};

} // eagle

#endif //EAGLE_VULKAN_SEMAPHORE_H
