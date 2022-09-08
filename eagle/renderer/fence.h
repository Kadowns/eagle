//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_FENCE_H
#define EAGLE_FENCE_H

#include <cstdint>
#include <limits>
#include <chrono>

namespace eagle {

/// Used to synchronize the cpu with gpu queue operations
class Fence {
public:

    virtual ~Fence() = default;

    // Blocks current thread until fence is signaled or the timeout is reached
    // timeout specified in milli seconds
    // returns true in case the fence becomes signaled, false if timeout was reached
    virtual bool wait(std::chrono::milliseconds timeout) = 0;

    // Overload to wait indefinably
    inline bool wait() {
        return wait(std::chrono::milliseconds(std::numeric_limits<uint64_t>::max()));
    }

    // Sets fence to an unsignaled state
    virtual void reset() = 0;

};

}

#endif //EAGLE_FENCE_H
