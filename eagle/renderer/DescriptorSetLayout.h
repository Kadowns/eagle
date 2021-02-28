//
// Created by Novak on 02/08/2019.
//

#ifndef EAGLE_DESCRIPTORSETLAYOUT_H
#define EAGLE_DESCRIPTORSETLAYOUT_H

#include "RenderingCore.h"
#include "DescriptorItem.h"

namespace eagle {

struct DescriptorBindingMemberDescription {
    std::string name;
    DataType type;
    uint32_t size;
    uint32_t offset;
};

struct DescriptorBindingDescription {
    std::string name;
    uint32_t binding;
    DescriptorType descriptorType;
    ShaderStage shaderStage;
    size_t size;
    std::map<std::string, DescriptorBindingMemberDescription> members;
};


class DescriptorSetLayout {
public:
    virtual const std::vector<DescriptorBindingDescription>& bindings() = 0;

};


}



#endif //EAGLE_DESCRIPTORSETLAYOUT_H
