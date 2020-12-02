//
// Created by Novak on 5/2/2020.
//

#ifndef EAGLE_COLLISIONSYSTEM_H
#define EAGLE_COLLISIONSYSTEM_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/PhysicsSettings.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/physics/Collider.h>
#include <eagle/engine/systems/physics/collision/Collision.h>
#include <eagle/engine/components/physics/CollisionSettings.h>

EG_ENGINE_BEGIN

class CollisionSystem : public entityx::System<CollisionSystem>, public entityx::Receiver<CollisionSystem> {
public:
    struct Body {
        Body(Rigidbody& rigidbody, Collider& collider) :  rigidbody(rigidbody), collider(collider) {}
        Rigidbody& rigidbody;
        Collider& collider;
    };
public:
    virtual void configure(entityx::EntityManager &entities, entityx::EventManager &events) override;
    virtual void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override;
    void receive(const OnPhysicsUpdate& ev);
    void receive(const entityx::ComponentAddedEvent<Collider> &ev);
    void receive(const entityx::ComponentRemovedEvent<Collider> &ev);
    void pre_solve(const Collision &collision, CollisionSettings::VelocityState &vsA,
                   CollisionSettings::VelocityState &vsB, float dt);
    void solve(const Collision &collision, CollisionSettings::VelocityState &vsA, CollisionSettings::VelocityState &vsB);
    void initialize_entity_collider(entityx::Entity e);

    //from qub3 and box2d
    inline void compute_basis(const glm::vec3 &a, glm::vec3 *__restrict b, glm::vec3 *__restrict c) {
        // Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
        // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735027. This means that at least one component of a
        // unit vector must be greater or equal to 0.57735027. Can use SIMD select operation.

        if (std::abs(a.x) >= 0.57735027) {
            //b->Set( a.y, -a.x, r32( 0.0 ) );
            b->x = a.y;
            b->y = -a.x;
            b->z = 0.0;
        } else {
//            b->Set( r32( 0.0 ), a.z, -a.y );
            b->x = 0.0;
            b->y = a.z;
            b->z = -a.y;
        }

        *b = glm::normalize(*b);
        *c = glm::cross(a, *b);
    }
};

EG_ENGINE_END

#endif //EAGLE_COLLISIONSYSTEM_H
