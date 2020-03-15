//
// Created by Novak on 17/11/2019.
//

#ifndef EAGLE_LIGHTS_H
#define EAGLE_LIGHTS_H

#include "eagle/engine/GlobalDefs.h"
#include "eagle/engine/EntityManager.h"

EG_ENGINE_BEGIN

struct DirectionalLight : Component<DirectionalLight>{

    DirectionalLight() : direction(1.0f, 0, 0), color(0), intensity(0) {}

    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
    alignas(4) float intensity;
};



EG_ENGINE_END

#endif //EAGLE_LIGHTS_H
