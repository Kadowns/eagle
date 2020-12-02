//
// Created by Novak on 5/4/2020.
//

#ifndef EAGLE_COLLISION_H
#define EAGLE_COLLISION_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/Collider.h>

EG_ENGINE_BEGIN

struct Collision {

    enum {
        COLLIDING_BIT = BIT(1),
        WAS_COLLIDING_BIT = BIT(2),
    };

    struct Contact{
        glm::vec3 position, rA, rB;
        float penetration;
        float normalMass, tangentMass[2];
        float bias;
        float normalImpulse, tangentImpulse[2];
        int32_t key;
    };

    Collision(entityx::Entity eA,
              entityx::Entity eB) :
              eA(eA),
              eB(eB),
              colA(eA.component<Collider>()),
              colB(eB.component<Collider>()),
              rbA(eA.component<Rigidbody>()),
              rbB(eB.component<Rigidbody>()) {
                  restitution = std::max(rbA->restitution, rbB->restitution);
                  friction = std::sqrt(rbA->friction * rbB->friction);
              }


    mutable std::vector<Contact> contacts;
    entityx::Entity eA, eB;
    entityx::ComponentHandle<Collider> colA, colB;
    entityx::ComponentHandle<Rigidbody> rbA, rbB;
    mutable glm::vec3 normal;
    glm::vec3 tangentVectors[2];
    float restitution;
    float friction;
    mutable int32_t phase;

    inline bool contains(const entityx::Entity& e){
        return eA == e || eB == e;
    }

    inline operator bool() const {
        return !contacts.empty();
    }

    inline bool operator==(const Collision& rhs) const {
        return (eA == rhs.eA && eB == rhs.eB) || (eA == rhs.eB && eB == rhs.eA);
    }
};

struct OnCollisionEnter {
    OnCollisionEnter(const Collision& collision) : collision(collision) {}
    Collision collision;
};

struct OnCollisionStay {
    OnCollisionStay(const Collision& collision) : collision(collision) {}
    Collision collision;
};

struct OnCollisionExit {
    OnCollisionExit(const Collision& collision) : collision(collision) {}
    Collision collision;
};

EG_ENGINE_END

#endif //EAGLE_COLLISION_H
