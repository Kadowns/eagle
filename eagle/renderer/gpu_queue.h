//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_GPU_QUEUE_H
#define EAGLE_GPU_QUEUE_H

#include <eagle/renderer/command_buffer.h>
#include <eagle/renderer/semaphore.h>
#include <eagle/renderer/fence.h>

#include <span>

namespace eagle {

enum class GPUQueueType {
    PRESENT = 0,
    RENDER = 1,
    COMPUTE = 2
};

struct GPUQueueSubmitInfo {
    std::span<CommandBuffer*> commandBuffers;
    std::span<Semaphore*> waitSemaphores;
    std::span<PipelineStageFlags> waitStages;
    std::span<Semaphore*> signalSemaphores;
    Fence* fence = nullptr;
};

struct GPUQueueCreateInfo {
    GPUQueueType type;
    std::string name;
    float priority;
};

class GPUQueue {
public:

public:
    GPUQueue(const GPUQueueCreateInfo& info) : m_info(info) {}
    virtual ~GPUQueue() = default;
    virtual void submit(const GPUQueueSubmitInfo& submitInfo) = 0;
protected:
    GPUQueueCreateInfo m_info;
};

}

#endif //EAGLE_GPU_QUEUE_H
