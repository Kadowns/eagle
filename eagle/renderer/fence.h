//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_FENCE_H
#define EAGLE_FENCE_H

#include <cstdint>
#include <limits>

namespace eagle {

/// Used to synchronize the cpu with gpu queue operations
class Fence {
public:

    // Blocks current thread until fence is signaled or the timeout is reached
    // timeout specified in milli seconds
    // returns true in case the fence becomes signaled, false if timeout was reached
    virtual bool wait(uint64_t timeout) = 0;

    // Overload to wait indefinably
    inline bool wait() { return wait(std::numeric_limits<uint64_t>::max()); }

    // Returns true if signaled, false if not
    virtual bool signaled() const = 0;

    // Sets fence to an unsignaled state
    virtual bool reset() = 0;

};

}

#endif //EAGLE_FENCE_H
