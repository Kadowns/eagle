//
// Created by Novak on 23/03/2020.
//

#ifndef EAGLE_COMPUTESHADER_H
#define EAGLE_COMPUTESHADER_H

#include "renderer_global_definitions.h"
#include "descriptor_set_layout.h"
#include "image.h"

namespace eagle {

class ComputeShader {

public:
    ComputeShader() = default;
    virtual ~ComputeShader() = default;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    virtual void update_image(uint32_t binding, const std::shared_ptr <ImageView>& image) = 0;
    virtual void update_descriptors() = 0;
    virtual void update_push_constants(void* data, size_t size) = 0;

    virtual void create_pipeline() = 0;
    virtual void cleanup_pipeline() = 0;

    virtual void join() = 0;

};

}

#endif //EAGLE_COMPUTESHADER_H
