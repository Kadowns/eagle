//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_APPLICATIONDELEGATE_H
#define EAGLE_APPLICATIONDELEGATE_H

#include <eagle/CoreGlobalDefinitions.h>

EG_BEGIN

class ApplicationDelegate {
public:
    virtual ~ApplicationDelegate() = default;
    virtual void init() = 0;
    virtual void step() = 0;
    virtual void destroy() = 0;
};

EG_END

#endif //EAGLE_APPLICATIONDELEGATE_H
