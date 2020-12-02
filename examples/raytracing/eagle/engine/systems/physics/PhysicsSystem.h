//
// Created by Novak on 4/24/2020.
//

#ifndef EAGLE_PHYSICSSYSTEM_H
#define EAGLE_PHYSICSSYSTEM_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/Box.h>

EG_ENGINE_BEGIN

class PhysicsSystem : public entityx::System<PhysicsSystem>, public entityx::Receiver<PhysicsSystem> {
public:
    virtual void configure(entityx::EntityManager &entities, entityx::EventManager &events) override;
    virtual void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
    void receive(const entityx::ComponentAddedEvent<Rigidbody>& ev);
private:
    void update_physics(entityx::EntityManager &entities, entityx::EventManager &events, float dt);
    static void integrate(glm::quat& t, const glm::vec3& dv, float dt);
};

EG_ENGINE_END

#endif //EAGLE_PHYSICSSYSTEM_H
