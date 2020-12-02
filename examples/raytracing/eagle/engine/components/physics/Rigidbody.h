//
// Created by Novak on 4/24/2020.
//

#ifndef EAGLE_RIGIDBODY_H
#define EAGLE_RIGIDBODY_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Rigidbody {
    enum class Mode {
        DYNAMIC = 0,
        STATIC = 1
    };


    struct Transform {
        glm::vec3 position;
        glm::quat rotation;
    };


    Rigidbody(float mass = 1.0f, float drag = 0.5f, float restitution = 0.2f, float friction = 0.4f, Mode mode = Mode::DYNAMIC)
    : velocity(0), force(0), angularVelocity(0), torque(0), mass(mass), inverseMass(mass != 0 ? 1 / mass : 0), drag(drag), restitution(restitution), friction(friction), mode(mode) {}
    glm::vec3 velocity, force;
    glm::vec3 angularVelocity, torque;
    float mass, inverseMass;
    float drag;
    float restitution;
    float friction;
    glm::mat3 inverseInertiaModel, inverseInertiaWorld;
    Transform previous, current;
    Mode mode;

    inline void apply_force(const glm::vec3& f){
        force += mass * f;
    }

    inline void apply_torque(const glm::vec3& t){
        torque += t;
    }

    inline void apply_force_at_world_point(const glm::vec3& f, const glm::vec3 &point) {
        force += mass * f;
        torque += glm::cross(point - current.position, f);
    }

    inline void apply_impulse_at_world_point(const glm::vec3& impulse, const glm::vec3& point){
        velocity += inverseMass * impulse;
        angularVelocity += inverseInertiaModel * glm::cross(point - current.position, impulse);
    }
};

EG_ENGINE_END

#endif //EAGLE_RIGIDBODY_H
