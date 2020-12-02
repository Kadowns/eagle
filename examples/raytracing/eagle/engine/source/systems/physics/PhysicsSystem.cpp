//
// Created by Novak on 4/24/2020.
//

#include <eagle/engine/systems/physics/PhysicsSystem.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/physics/PhysicsSettings.h>
#include <eagle/engine/components/SingletonComponent.h>

EG_ENGINE_BEGIN

void PhysicsSystem::configure(entityx::EntityManager &entities, entityx::EventManager &events) {
    events.subscribe<entityx::ComponentAddedEvent<Rigidbody>>(*this);
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<Rigidbody> rigidbody;
    for (auto e : entities.entities_with_components<Transform, Rigidbody>(transform, rigidbody)){
        rigidbody->current.position = transform->position();
        rigidbody->current.rotation = transform->rotation();
        rigidbody->previous = rigidbody->current;
    }
}

void PhysicsSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {


    auto& settings = SingletonComponent::get<PhysicsSettings>();
    settings.accumulator += dt;
    settings.accumulator = std::min(settings.accumulator, 0.2f);

    float fixedStep = settings.fixedStep * Time::time_scale();
    if (fixedStep > 0) {
        while (settings.accumulator > fixedStep) {
            update_physics(entities, events, fixedStep);
            settings.accumulator -= fixedStep;
        }
    }

    settings.alpha = settings.accumulator / fixedStep;

    //transform interpolation
    entityx::ComponentHandle<Transform> transform;
    entityx::ComponentHandle<Rigidbody> rigidbody;
    for (auto e : entities.entities_with_components<Transform, Rigidbody>(transform, rigidbody)){
        transform->set_position(glm::mix(rigidbody->previous.position, rigidbody->current.position, settings.alpha));
        transform->set_rotation(glm::mix(rigidbody->previous.rotation, rigidbody->current.rotation, settings.alpha));
    }
}

void PhysicsSystem::update_physics(entityx::EntityManager &entities, entityx::EventManager &events, float dt) {
    auto& settings = SingletonComponent::get<PhysicsSettings>();

    entityx::ComponentHandle<Rigidbody> rigidbody;
    for (auto e : entities.entities_with_components<Rigidbody>(rigidbody)){
        if (rigidbody->mode == Rigidbody::Mode::STATIC){
            continue;
        }

        rigidbody->apply_force(settings.gravity);

        glm::mat3 rotation = glm::mat3_cast(rigidbody->current.rotation);
        rigidbody->inverseInertiaWorld = glm::transpose(rotation) * rigidbody->inverseInertiaModel * rotation;

        rigidbody->velocity += (rigidbody->force * rigidbody->inverseMass) * dt;
        rigidbody->angularVelocity += (rigidbody->inverseInertiaWorld * rigidbody->torque) * dt;

        //drag
//        rigidbody->velocity *= (1 - dt * rigidbody->drag);
//        rigidbody->angularVelocity *= (1 - dt * rigidbody->drag);

        rigidbody->velocity *=  1.0  / ( 1.0  + dt * rigidbody->drag);
        rigidbody->angularVelocity *=  1.0  / ( 1.0  + dt * rigidbody->drag);

        rigidbody->force = glm::vec3(0);
        rigidbody->torque = glm::vec3(0);
    }

    events.emit<OnPhysicsUpdate>(entities, events, dt);

    for (auto e : entities.entities_with_components<Rigidbody>(rigidbody)){
        if (rigidbody->mode == Rigidbody::Mode::STATIC){
            continue;
        }

        rigidbody->previous = rigidbody->current;
        rigidbody->current.position += rigidbody->velocity * dt;
        integrate(rigidbody->current.rotation, rigidbody->angularVelocity, dt);
    }
}

void PhysicsSystem::receive(const entityx::ComponentAddedEvent<Rigidbody>& ev) {
    assert(ev.entity.has_component<Transform>());
    entityx::Entity e = ev.entity;
    entityx::ComponentHandle<Transform> transform = e.component<Transform>();
    entityx::ComponentHandle<Rigidbody> rigidbody = e.component<Rigidbody>();
    rigidbody->current.position = transform->position();
    rigidbody->current.rotation = transform->rotation();
    rigidbody->previous = rigidbody->current;
}

void PhysicsSystem::integrate(glm::quat &t, const glm::vec3 &dv, float dt) {
    glm::quat q(0.0, dv.x * dt, dv.y * dt, dv.z * dt);

    q *= t;

    t.x += q.x * 0.5;
    t.y += q.y * 0.5;
    t.z += q.z * 0.5;
    t.w += q.w * 0.5;

    t = glm::normalize(t);
}

EG_ENGINE_END
