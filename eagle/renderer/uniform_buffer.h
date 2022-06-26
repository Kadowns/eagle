//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFER_H
#define EAGLE_UNIFORMBUFFER_H

#include <eagle/renderer/gpu_buffer.h>
#include <eagle/renderer/descriptor_item.h>

namespace eagle {

class UniformBuffer : public Descriptor, public GPUBuffer {
public:
    UniformBuffer(void* data, size_t size) : GPUBuffer(data, size) {}

    // from eagle::Descriptor
    DescriptorType type() const override {
        return DescriptorType::UNIFORM_BUFFER;
    }
};

}
#endif //EAGLE_UNIFORMBUFFER_H
