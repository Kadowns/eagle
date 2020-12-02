//
// Created by Novak on 09/04/2020.
//

#ifndef EAGLE_LIGHTSYSTEM_H
#define EAGLE_LIGHTSYSTEM_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

class LightSystem : public entityx::System<LightSystem> {
public:
    virtual void configure(entityx::EntityManager &entities, entityx::EventManager &events) override;

    virtual void
    update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
};

EG_ENGINE_END

#endif //EAGLE_LIGHTSYSTEM_H
