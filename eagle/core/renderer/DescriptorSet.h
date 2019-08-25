//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include "eagle/core/Core.h"
#include "UniformBuffer.h"

EG_BEGIN

class DescriptorSet {
public:
    virtual ~DescriptorSet() = default;
};

EG_END

#endif //EAGLE_DESCRIPTORSET_H
