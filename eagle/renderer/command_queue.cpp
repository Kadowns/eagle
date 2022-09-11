//
// Created by Ricardo on 10/09/2022.
//

#include <eagle/renderer/command_queue.h>

namespace eagle {

void CommandQueue::submit(const CommandBufferSubmitInfo& submitInfo, Fence* fence) {
    CommandBufferSubmitInfo submitInfos[] = {submitInfo};
    submit(submitInfos, fence);
}

}