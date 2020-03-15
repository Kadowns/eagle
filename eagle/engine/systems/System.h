//
// Created by Novak on 17/11/2019.
//

#ifndef EAGLE_SYSTEM_H
#define EAGLE_SYSTEM_H

#include "eagle/engine/GlobalDefs.h"

EG_ENGINE_BEGIN

class BaseSystem {
public:

    virtual void update() = 0;

protected:

    static size_t s_familyCounter;
};

template<typename Derived>
class System : BaseSystem {
public:




    static size_t family() {
        static s_family = s_familyCounter++;
        return s_family;
    }
};


EG_ENGINE_END

#endif //EAGLE_SYSTEM_H
