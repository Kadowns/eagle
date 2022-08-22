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

};

class VulkanSemaphore : public Semaphore {
public:

    VkSemaphore native_semaphore(uint32_t frame) { return m_semaphores[frame]; }

private:
    std::vector<VkSemaphore> m_semaphores;
};

} // eagle

#endif //TRIANGLE_VULKAN_SEMAPHORE_H
