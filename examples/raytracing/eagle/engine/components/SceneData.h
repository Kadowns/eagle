//
// Created by Novak on 04/04/2020.
//

#ifndef EAGLE_SCENEDATA_H
#define EAGLE_SCENEDATA_H

#include <eagle/engine/EngineGlobalDefinitions.h>



EG_ENGINE_BEGIN

struct SceneData {
    int maxSphereCount = 100;
    glm::vec2 sphereSizes = glm::vec2(2, 6);
    float spherePlacementRadius = 30.0f;
    float metalPercent = 0.5f;
    glm::vec3 cubeRotation;
};


EG_ENGINE_END

#endif //EAGLE_SCENEDATA_H
