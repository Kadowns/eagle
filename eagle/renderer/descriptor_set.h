//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include <eagle/core_global_definitions.h>
#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/descriptor_set_layout.h>

namespace eagle {

struct DescriptorSetCreateInfo {
    DescriptorSetLayout* layout;
    std::vector<Descriptor*> descriptors;
};

class DescriptorSet {
public:
    explicit DescriptorSet(DescriptorSetCreateInfo info) : m_info(std::move(info)) {}
    virtual ~DescriptorSet() = default;
    virtual void update() = 0;
    size_t size() const { return m_info.descriptors.size(); }
    Descriptor* operator[](size_t binding) { return m_info.descriptors[binding]; }
protected:
    DescriptorSetCreateInfo m_info;

};

}

#endif //EAGLE_DESCRIPTORSET_H
