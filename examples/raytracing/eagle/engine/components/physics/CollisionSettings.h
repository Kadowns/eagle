//
// Created by Novak on 26/05/2020.
//

#ifndef EAGLE_COLLISIONSETTINGS_H
#define EAGLE_COLLISIONSETTINGS_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/systems/physics/collision/Collision.h>

EG_ENGINE_BEGIN

struct CollisionSettings {

    struct VelocityState {
        glm::vec3 v, w;
    };

    std::vector<Collision> collisions;
    std::unordered_map<entityx::Entity, VelocityState> velocities;
};

EG_ENGINE_END

#endif //EAGLE_COLLISIONSETTINGS_H
