//
// Created by Novak on 11/04/2020.
//

#ifndef EAGLE_SPHERE_H
#define EAGLE_SPHERE_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Sphere {
    float radius;
    glm::vec3 albedo;
    glm::vec3 specular;
};

EG_ENGINE_END

#endif //EAGLE_SPHERE_H
