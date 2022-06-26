//
// Created by Ricardo on 4/5/2021.
//

#ifndef EAGLE_GPU_BUFFER_H
#define EAGLE_GPU_BUFFER_H

#include <eagle/memory/buffer.h>

namespace eagle {

class GPUBuffer : public Buffer {
public:
    //inherits Buffer constructors
    using Buffer::Buffer;

    //makes changes available to the gpu
    virtual void flush() = 0;
};

}

#endif //EAGLE_GPU_BUFFER_H
