//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include "eagle/core/CoreGlobalDefinitions.h"
#include "UniformBuffer.h"

EG_BEGIN

class DescriptorSet {
public:
    virtual ~DescriptorSet() = default;
    virtual void update(const std::vector<Reference<DescriptorItem>>& descriptorItems) = 0;
};

EG_END

#endif //EAGLE_DESCRIPTORSET_H
