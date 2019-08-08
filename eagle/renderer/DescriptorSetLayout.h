//
// Created by Novak on 02/08/2019.
//

#ifndef EAGLE_DESCRIPTORSETLAYOUT_H
#define EAGLE_DESCRIPTORSETLAYOUT_H

#include "RenderingCore.h"
#include "DescriptorItem.h"

_EAGLE_BEGIN


struct DescriptorBinding {
    uint32_t binding;
    EG_DESCRIPTOR_TYPE descriptorType;
    EG_SHADER_STAGE shaderStage;
};


class DescriptorSetLayout {
protected:
    virtual void create(const std::vector<DescriptorBinding> &bindings) = 0;

};


_EAGLE_END



#endif //EAGLE_DESCRIPTORSETLAYOUT_H
