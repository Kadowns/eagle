//
// Created by Ricardo on 10/09/2022.
//

#ifndef EAGLE_COMMANDQUEUE_H
#define EAGLE_COMMANDQUEUE_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <span>

namespace eagle {

class Semaphore;
class Fence;
class CommandBuffer;
class Image;

enum class CommandQueueType {
    UNDEFINED = 0,
    PRESENT = 1,
    GRAPHICS = 2,
    COMPUTE = 3
};

struct CommandBufferSubmitInfo {
    std::span<CommandBuffer*> commandBuffers;
    std::span<Semaphore*> waitSemaphores;
    std::span<PipelineStageFlags> waitStages;
    std::span<Semaphore*> signalSemaphores;
};

class CommandQueue {
public:

    void release_ownership(CommandQueue* newOwner, Image* image, PipelineStageFlags srcStage, PipelineStageFlags dstStage)

    void submit(const CommandBufferSubmitInfo& submitInfo, Fence* fence);

    virtual void submit(std::span<CommandBufferSubmitInfo> submitInfos, Fence* fence) = 0;

};

}

#endif //EAGLE_COMMANDQUEUE_H
