//
// Created by Novak on 09/04/2020.
//

#ifndef EAGLE_DIRECTIONALLIGHT_H
#define EAGLE_DIRECTIONALLIGHT_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct DirectionalLight {
    glm::vec3 direction;
    float intensity = 1.0f;
};

struct OnLightUpdate {
    OnLightUpdate(const DirectionalLight& light) : light(light) {}
    const DirectionalLight& light;
};

EG_ENGINE_END

#endif //EAGLE_DIRECTIONALLIGHT_H
