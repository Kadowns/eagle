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
    virtual ~DescriptorSet() = default;
    virtual void update(const std::vector<std::shared_ptr<DescriptorItem>>& descriptorItems) = 0;
};

}

#endif //EAGLE_DESCRIPTORSET_H
