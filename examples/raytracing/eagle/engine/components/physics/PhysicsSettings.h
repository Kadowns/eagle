//
// Created by Novak on 5/2/2020.
//

#ifndef EAGLE_PHYSICSSETTINGS_H
#define EAGLE_PHYSICSSETTINGS_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct PhysicsSettings {
    float accumulator = 0;
    float alpha = 0;
    float fixedStep = 0.01f;
    glm::vec3 gravity;
};

struct OnPhysicsUpdate {
    OnPhysicsUpdate(entityx::EntityManager& entities, entityx::EventManager& events, float dt) : entities(entities), events(events), dt(dt) {}
    entityx::EntityManager& entities;
    entityx::EventManager& events;
    float dt;
};

EG_ENGINE_END

#endif //EAGLE_PHYSICSSETTINGS_H
