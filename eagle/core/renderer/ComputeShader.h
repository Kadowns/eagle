//
// Created by Novak on 23/03/2020.
//

#ifndef EAGLE_COMPUTESHADER_H
#define EAGLE_COMPUTESHADER_H

#include "RenderingCore.h"
#include "DescriptorSetLayout.h"
#include "Image.h"

EG_BEGIN

class ComputeShader {

public:
    ComputeShader() = default;
    virtual ~ComputeShader() = default;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) = 0;

    virtual void update_descriptor_items(const std::vector<Reference<DescriptorItem>>& descriptorItems) = 0;
    virtual void set_image(const std::string& name, const Reference<Image>& image) = 0;

    virtual void create_pipeline() = 0;
    virtual void cleanup_pipeline() = 0;

};

EG_END

#endif //EAGLE_COMPUTESHADER_H
