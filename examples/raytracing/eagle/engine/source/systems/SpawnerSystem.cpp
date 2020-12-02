//
// Created by Novak on 29/05/2020.
//

#include <eagle/engine/systems/SpawnerSystem.h>
#include <eagle/engine/components/Spawner.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Camera.h>
#include <eagle/engine/components/SceneData.h>
#include <eagle/engine/components/SingletonComponent.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/Box.h>

#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/physics/Collider.h>
#include <eagle/engine/components/physics/shapes/SphereCollider.h>
#include <eagle/engine/components/physics/shapes/BoxCollider.h>

EG_ENGINE_BEGIN

void SpawnerSystem::configure(entityx::EntityManager &entities, entityx::EventManager &events) {

}

void SpawnerSystem::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {

    auto& spawner = SingletonComponent::get<Spawner>();

    entities.each<Transform, Camera>([&entities, &spawner](entityx::Entity e, Transform& transform, Camera& camera){

        if (Input::instance().key_pressed(EG_KEY_G)){
            SceneData& scene = SingletonComponent::get<SceneData>();
            glm::vec3 color = glm::vec3(Random::value(), Random::value(), Random::value());
            bool metal = Random::value() < scene.metalPercent;

            auto s = entities.create();
            auto sphere = s.assign<Sphere>();
            sphere->radius = Random::range(0.3, 8.0f);
            sphere->albedo = metal ? glm::vec3(0.01f) : glm::vec3(color.r, color.g, color.b);
            sphere->specular = metal ? glm::vec3(color.r, color.g, color.b) : glm::vec3(0.01f);
            auto tr = s.assign<Transform>(transform);
            tr->translate(transform.front() * sphere->radius * 2.0f);
            if (spawner.isStatic){
                s.assign<Rigidbody>(0, 0, 0, 0.4, Rigidbody::Mode::STATIC);
            }
            else {
                auto rigidbody = s.assign<Rigidbody>();
                rigidbody->velocity = transform.front() * 40.0f;
            }

            s.assign<Collider>(std::make_shared<SphereCollider>(sphere->radius));
        }

        if (Input::instance().key_pressed(EG_KEY_B)){
            SceneData& scene = SingletonComponent::get<SceneData>();

            auto s = entities.create();
            auto tr = s.assign<Transform>();
//            tr->set_position(glm::vec3(0, 40, 0));
            tr->set_position((transform.position()));
            tr->set_rotation(glm::radians(spawner.rotation));

            auto box = s.assign<Box>();
            box->radius = spawner.radius;
            box->albedo = glm::vec3(0.1f);
            box->specular = glm::vec3(0.8f);
            tr->translate(transform.front() * box->radius.z * 2.0f);

            if (spawner.isStatic){
                s.assign<Rigidbody>(0, 0, 0, 0.4, Rigidbody::Mode::STATIC);
            }
            else {
                auto rigidbody = s.assign<Rigidbody>();
                rigidbody->mode = Rigidbody::Mode::DYNAMIC;
                rigidbody->velocity = transform.front() * 15.0f;
                rigidbody->friction = 0.4f;
            }


            s.assign<Collider>(std::make_shared<BoxCollider>(box->radius));
        }
    });

}



EG_ENGINE_END

