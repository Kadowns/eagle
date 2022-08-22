//
// Created by Ricardo on 26/06/2022.
//

#ifndef EAGLE_SEMAPHORE_H
#define EAGLE_SEMAPHORE_H

namespace eagle {
/// Opaque class to synchronize gpu queue operations
class Semaphore {
public:
    virtual ~Semaphore() = default;
};

}

#endif //EAGLE_SEMAPHORE_H
