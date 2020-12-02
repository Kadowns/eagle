//
// Created by Novak on 5/2/2020.
//

#include <eagle/engine/systems/physics/CollisionSystem.h>
#include <eagle/engine/systems/physics/collision/Collision.h>
#include <eagle/engine/systems/physics/collision/CollisionManager.h>
#include <eagle/engine/components/physics/Collider.h>
#include <eagle/engine/components/physics/CollisionSettings.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/SingletonComponent.h>
#include <eagle/engine/components/physics/PhysicsSettings.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/SceneManager.h>

EG_ENGINE_BEGIN

void CollisionSystem::configure(entityx::EntityManager &entities, entityx::EventManager &events) {
    events.subscribe<OnPhysicsUpdate>(*this);
    events.subscribe<entityx::ComponentAddedEvent<Collider>>(*this);
    events.subscribe<entityx::ComponentRemovedEvent<Collider>>(*this);
    CollisionManager::configure();

    auto& collisions = SingletonComponent::get<CollisionSettings>().collisions;

    for (auto e : entities.entities_with_components<Collider, Rigidbody>()) {
        initialize_entity_collider(e);
    }
}

void CollisionSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {

}

void CollisionSystem::initialize_entity_collider(entityx::Entity e) {
    assert(e.has_component<Collider>());
    assert(e.has_component<Rigidbody>());
    auto cA = e.component<Collider>();
    auto rbA = e.component<Rigidbody>();
    cA->shape->compute_inertia(*rbA);

    auto& entities = SceneManager::current_scene().entities;
    auto& settings = SingletonComponent::get<CollisionSettings>();
    for (auto eB : entities.entities_with_components<Collider, Rigidbody>()){
        if (e == eB){
            continue;
        }

        auto cB = eB.component<Collider>();
        if (!(cA->layer & cB->layer)){
            continue;
        }

        auto rbB = eB.component<Rigidbody>();
        if (rbA->mode == Rigidbody::Mode::STATIC && rbB->mode == Rigidbody::Mode::STATIC){
            continue;
        }


        settings.collisions.emplace_back(e, eB);
    }
    settings.velocities.clear();
}

void CollisionSystem::receive(const entityx::ComponentAddedEvent<Collider> &ev) {
    initialize_entity_collider(ev.entity);
}

void CollisionSystem::receive(const entityx::ComponentRemovedEvent<Collider> &ev) {
    auto e = ev.entity;
    auto& settings= SingletonComponent::get<CollisionSettings>();
    settings.collisions.erase(std::remove_if(settings.collisions.begin(), settings.collisions.end(), [e](Collision& collision){
        return collision.contains(e);
    }), settings.collisions.end());
    settings.velocities.clear();
}


void CollisionSystem::receive(const OnPhysicsUpdate& ev) {

    auto& settings = SingletonComponent::get<CollisionSettings>();

    settings.velocities.clear();
    for (auto e : ev.entities.entities_with_components<Collider, Rigidbody>()){
        entityx::ComponentHandle<Collider> c = e.component<Collider>();
        entityx::ComponentHandle<Rigidbody> rb = e.component<Rigidbody>();
        c->shape->compute_aabb(rb->current);
        CollisionSettings::VelocityState velocities = {};
        velocities.v = rb->velocity;
        velocities.w = rb->angularVelocity;
        settings.velocities[e] = velocities;
    }

    for (auto& collision : settings.collisions){
        if (CollisionManager::test_aabb(collision.colA->shape->aabb(), collision.colB->shape->aabb())){

            Collision oldCollision = collision;
            glm::vec3 ot0 = oldCollision.tangentVectors[0];
            glm::vec3 ot1 = oldCollision.tangentVectors[1];
            CollisionManager::test_collision(collision);

            if (collision){
                compute_basis(collision.normal, collision.tangentVectors, collision.tangentVectors + 1);
                for (auto& contact : collision.contacts){
                    contact.normalImpulse = 0;
                    contact.tangentImpulse[0] = 0;
                    contact.tangentImpulse[1] = 0;

                    for (auto& oldContact : oldCollision.contacts){
                        if (contact.key == oldContact.key){
                            contact.normalImpulse = oldContact.normalImpulse;
                            // Attempt to re-project old friction solutions
                            glm::vec3 friction = ot0 * oldContact.tangentImpulse[0] + ot1 * oldContact.tangentImpulse[1];
                            contact.tangentImpulse[0] = glm::dot(friction, collision.tangentVectors[0]);
                            contact.tangentImpulse[1] = glm::dot(friction, collision.tangentVectors[1]);
                            break;
                        }
                    }
                }

                pre_solve(collision, settings.velocities[collision.eA], settings.velocities[collision.eB], ev.dt);
                for (int i = 0; i < 8; i++){
                    solve(collision, settings.velocities[collision.eA], settings.velocities[collision.eB]);
                }
            }

            int32_t now_colliding = collision.phase & Collision::COLLIDING_BIT;
            int32_t was_colliding = collision.phase & Collision::WAS_COLLIDING_BIT;

            if (now_colliding && !was_colliding) {
                ev.events.emit<OnCollisionEnter>(collision);
            }
            else if (!now_colliding && was_colliding) {
                ev.events.emit<OnCollisionExit>(collision);
            }
            else if (now_colliding && was_colliding) {
                ev.events.emit<OnCollisionStay>(collision);
            }
        }
    }

    for (auto e : ev.entities.entities_with_components<Collider, Rigidbody>()){
        entityx::ComponentHandle<Collider> c = e.component<Collider>();
        entityx::ComponentHandle<Rigidbody> rb = e.component<Rigidbody>();

        if (rb->mode == Rigidbody::Mode::STATIC){
            continue;
        }

        rb->velocity = settings.velocities[e].v;
        rb->angularVelocity = settings.velocities[e].w;
    }
}


void CollisionSystem::pre_solve(const Collision &collision, CollisionSettings::VelocityState &vsA,
                                CollisionSettings::VelocityState &vsB, float dt) {
    glm::vec3 v1 = vsA.v;
    glm::vec3 w1 = vsA.w;
    glm::vec3 v2 = vsB.v;
    glm::vec3 w2 = vsB.w;

    for (auto& contact : collision.contacts) {

        glm::vec3 r1Cn = glm::cross( contact.rA, collision.normal );
        glm::vec3 r2Cn = glm::cross( contact.rB, collision.normal );
        float nm = collision.rbA->inverseMass + collision.rbB->inverseMass;
        float tm[ 2 ];
        tm[ 0 ] = nm;
        tm[ 1 ] = nm;

        nm += glm::dot(r1Cn, collision.rbA->inverseInertiaWorld * r1Cn) + glm::dot(r2Cn, collision.rbB->inverseInertiaWorld * r2Cn);
        contact.normalMass = nm != 0.0f ? 1 / nm : 0.0f;//inverse mass

        for (int i = 0; i < 2; ++i) {
            glm::vec3 raCt = glm::cross(collision.tangentVectors[i], contact.rA);
            glm::vec3 rbCt = glm::cross(collision.tangentVectors[i], contact.rB);
            tm[i] += glm::dot(raCt, collision.rbA->inverseInertiaWorld * raCt) +
                     glm::dot(rbCt, collision.rbB->inverseInertiaWorld * rbCt);
            contact.tangentMass[i] = tm[i] != 0.0f ? 1 / tm[i] : 0.0f;
        }


        contact.bias = -0.2f * (1.0  / dt) * std::min(0.0f, contact.penetration + 0.01f );


        glm::vec3 P = collision.normal * contact.normalImpulse;

        P += collision.tangentVectors[0] * contact.tangentImpulse[0];
        P += collision.tangentVectors[1] * contact.tangentImpulse[1];


        v1 -= P * collision.rbA->inverseMass;
        w1 -= collision.rbA->inverseInertiaWorld * glm::cross(contact.rA, P);


        v2 += P * collision.rbB->inverseMass;
        w2 += collision.rbB->inverseInertiaWorld * glm::cross(contact.rB, P);


        // Add in restitution bias
        float dv = glm::dot(v2 + glm::cross(w2, contact.rB) - v1 - glm::cross(w1, contact.rA), collision.normal);

        if (dv < -1.0f)
            contact.bias += -(collision.restitution) * dv;
    }
    vsA.v = v1;
    vsA.w = w1;
    vsB.v = v2;
    vsB.w = w2;
}

void CollisionSystem::solve(const Collision &collision, CollisionSettings::VelocityState &vsA,
                            CollisionSettings::VelocityState &vsB) {

    glm::vec3 v1 = vsA.v;
    glm::vec3 w1 = vsA.w;
    glm::vec3 v2 = vsB.v;
    glm::vec3 w2 = vsB.w;

    for (auto& contact : collision.contacts) {

        // relative velocity at contact
        glm::vec dv = v2 + glm::cross(w2, contact.rB) - v1 - glm::cross(w1, contact.rA);


        // Friction

        for (int i = 0; i < 2; ++i) {
            float lambda = -glm::dot(dv, collision.tangentVectors[i]) * contact.tangentMass[i];

            // Calculate frictional impulse
            float maxLambda = collision.friction * contact.normalImpulse;

            // Clamp frictional impulse
            float oldPT = contact.tangentImpulse[i];
            contact.tangentImpulse[i] = std::clamp(oldPT + lambda, -maxLambda, maxLambda);
            lambda = contact.tangentImpulse[i] - oldPT;

            // Apply friction impulse
            glm::vec3 impulse = collision.tangentVectors[i] * lambda;
            v1 -= collision.rbA->inverseMass * impulse;
            w1 -= collision.rbA->inverseInertiaWorld * glm::cross(contact.rA, impulse);


            v2 += collision.rbB->inverseMass * impulse;
            w2 += collision.rbB->inverseInertiaWorld * glm::cross(contact.rB, impulse);
        }


        // Normal impulse
        {
            dv = v2 + glm::cross(w2, contact.rB) - v1 - glm::cross(w1, contact.rA);

            // Normal impulse
            float vn = glm::dot(dv, collision.normal);

            // Factor in positional bias to calculate impulse scalar j
            float lambda = contact.normalMass * (-vn + contact.bias);

            // Clamp impulse
            float tempPN = contact.normalImpulse;
            contact.normalImpulse = std::max(tempPN + lambda, 0.0f);
            lambda = contact.normalImpulse - tempPN;

            // Apply impulse
            glm::vec3 impulse = collision.normal * lambda;
            v1 -= impulse * collision.rbA->inverseMass;
            w1 -= collision.rbA->inverseInertiaWorld * glm::cross(contact.rA, impulse);

            v2 += impulse * collision.rbB->inverseMass;
            w2 += collision.rbB->inverseInertiaWorld * glm::cross(contact.rB, impulse);
        }
    }
    vsA.v = v1;
    vsA.w = w1;
    vsB.v = v2;
    vsB.w = w2;
}


EG_ENGINE_END
