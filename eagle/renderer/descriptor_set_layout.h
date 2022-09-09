//
// Created by Novak on 02/08/2019.
//

#ifndef EAGLE_DESCRIPTORSETLAYOUT_H
#define EAGLE_DESCRIPTORSETLAYOUT_H

#include "renderer_global_definitions.h"
#include "descriptor_item.h"

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

struct DescriptorSetLayoutCreateInfo {
    std::vector<DescriptorBindingDescription> bindings;
};

class DescriptorSetLayout {
public:
    explicit DescriptorSetLayout(DescriptorSetLayoutCreateInfo info) :
    m_info(std::move(info)) {}

    virtual ~DescriptorSetLayout() = default;

    const DescriptorSetLayoutCreateInfo& info() const { return m_info; }

protected:
    DescriptorSetLayoutCreateInfo m_info;
};


}



#endif //EAGLE_DESCRIPTORSETLAYOUT_H
