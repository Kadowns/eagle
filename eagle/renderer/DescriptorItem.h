//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_DESCRIPTORITEM_H
#define EAGLE_DESCRIPTORITEM_H

#include "RenderingCore.h"

namespace eagle {

class DescriptorItem {
public:
    virtual ~DescriptorItem() = default;
    virtual DescriptorType type() const = 0;
};


}

#endif //EAGLE_DESCRIPTORITEM_H
