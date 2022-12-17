//
// Created by Ricardo on 14/08/2022.
//

#ifndef EAGLE_VULKAN_FENCE_H
#define EAGLE_VULKAN_FENCE_H

#include <eagle/renderer/fence.h>

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

namespace eagle {

struct VulkanFenceCreateInfo {
    VkDevice device;
};

class VulkanFence : public Fence {
public:
    explicit VulkanFence(const VulkanFenceCreateInfo& createInfo);
    ~VulkanFence() override;

    bool wait(std::chrono::milliseconds timeout) override;

    void acquire(VkFence fence);

    VkFence current_fence();

private:
    VulkanFenceCreateInfo m_createInfo;
    VkFence m_currentFence;
};

}

#endif //EAGLE_VULKAN_FENCE_H
