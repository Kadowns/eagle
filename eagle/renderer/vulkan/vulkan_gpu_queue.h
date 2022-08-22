//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_VULKAN_GPU_QUEUE_H
#define EAGLE_VULKAN_GPU_QUEUE_H

#include <eagle/renderer/gpu_queue.h>

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

namespace eagle {

struct VulkanGPUQueueCreateInfo {
    VkDevice device;
    uint32_t* currentFrame = nullptr;
};

class VulkanGPUQueue : public GPUQueue {
public:

    VulkanGPUQueue(const GPUQueueCreateInfo& createInfo, const VulkanGPUQueueCreateInfo& nativeCreateInfo);

    void submit(const GPUQueueSubmitInfo& submitInfo) override;

private:
    VulkanGPUQueueCreateInfo m_nativeCreateInfo;
    VkQueue m_queue;
};

} // eagle

#endif //EAGLE_VULKAN_GPU_QUEUE_H
