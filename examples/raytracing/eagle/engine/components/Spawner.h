//
// Created by Novak on 29/05/2020.
//

#ifndef EAGLE_SPAWNER_H
#define EAGLE_SPAWNER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Spawner {
    glm::vec3 rotation;
    glm::vec3 radius = glm::vec3(1);
    bool isStatic;
};

EG_ENGINE_END

#endif //EAGLE_SPAWNER_H
