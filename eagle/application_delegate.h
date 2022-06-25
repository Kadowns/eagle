//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_APPLICATIONDELEGATE_H
#define EAGLE_APPLICATIONDELEGATE_H

#include <eagle/core_global_definitions.h>

namespace eagle {

class ApplicationDelegate {
public:
    virtual ~ApplicationDelegate() = default;
    virtual void init() = 0;
    virtual void step() = 0;
};

}

#endif //EAGLE_APPLICATIONDELEGATE_H
