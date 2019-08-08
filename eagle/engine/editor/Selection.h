//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_SELECTION_H
#define EAGLE_SELECTION_H

#include "eagle/engine/EngineCore.h"

EG_ENGINE_BEGIN

class Selection {
public:
    static entityx::Entity current;
};

EG_ENGINE_END

#endif //EAGLE_SELECTION_H
