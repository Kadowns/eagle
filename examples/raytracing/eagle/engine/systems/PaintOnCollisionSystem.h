//
// Created by Novak on 06/06/2020.
//

#ifndef EAGLE_PAINTONCOLLISIONSYSTEM_H
#define EAGLE_PAINTONCOLLISIONSYSTEM_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/systems/physics/collision/Collision.h>

EG_ENGINE_BEGIN

class PaintOnCollisionSystem : public entityx::System<PaintOnCollisionSystem>, public entityx::Receiver<PaintOnCollisionSystem> {
public:
    virtual void configure(entityx::EntityManager &entities, entityx::EventManager &events) override {
        events.subscribe<OnCollisionEnter>(*this);
        events.subscribe<OnCollisionStay>(*this);
        events.subscribe<OnCollisionExit>(*this);
    }

    virtual void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {

    }

    void receive(const OnCollisionEnter& ev){
//        EG_DEBUG("Collision enter!");
    }

    void receive(const OnCollisionStay& ev){
//        EG_DEBUG("Collision stay!");
    }

    void receive(const OnCollisionExit& ev){
//        EG_DEBUG("Collision exit!");
    }
};


EG_ENGINE_END

#endif //EAGLE_PAINTONCOLLISIONSYSTEM_H
