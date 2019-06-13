//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include "eagle/core/Core.h"
#include "UniformBuffer.h"

_EAGLE_BEGIN

class DescriptorSet {

public:
    virtual void bind() = 0;

};

_EAGLE_END

#endif //EAGLE_DESCRIPTORSET_H
