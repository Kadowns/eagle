//
// Created by Ricardo on 14/08/2022.
//

#ifndef EAGLE_VULKAN_FENCE_H
#define EAGLE_VULKAN_FENCE_H

#include <eagle/renderer/fence.h>

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

namespace eagle {

class VulkanFence : public Fence {
public:

    VkFence native_fence(size_t index) { return m_fences[index]; }

private:
    std::vector<VkFence> m_fences;
};

}

#endif //EAGLE_VULKAN_FENCE_H
