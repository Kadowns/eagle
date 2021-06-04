//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include "eagle/core_global_definitions.h"
#include "uniform_buffer.h"

namespace eagle {

class DescriptorSet {
public:
    DescriptorSet(size_t descriptorCount) : m_descriptors(descriptorCount) {}
    DescriptorSet(const std::vector<WeakPointer<DescriptorItem>>& descriptors) : m_descriptors(descriptors) {}
    virtual ~DescriptorSet() = default;
    virtual void update() = 0;

    inline size_t size() const { return m_descriptors.size(); }
    inline const std::vector<WeakPointer<DescriptorItem>>& descriptors() const { return m_descriptors; }
    inline WeakPointer<DescriptorItem>& operator[](size_t binding) { return m_descriptors[binding]; }
protected:
    std::vector<WeakPointer<DescriptorItem>> m_descriptors;
};

}

#endif //EAGLE_DESCRIPTORSET_H
