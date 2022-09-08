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
    uint32_t frameCount;
    uint32_t* currentFrame;
};

class VulkanFence : public Fence {
public:
    explicit VulkanFence(const VulkanFenceCreateInfo& createInfo);
    ~VulkanFence() override;

    bool wait(std::chrono::milliseconds timeout) override;

    void reset() override;

    VkFence native_fence(size_t index);

private:
    VulkanFenceCreateInfo m_createInfo;
    std::vector<VkFence> m_fences;
};

}

#endif //EAGLE_VULKAN_FENCE_H
