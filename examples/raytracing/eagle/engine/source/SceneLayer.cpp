//
// Created by Novak on 4/21/2020.
//

#include <eagle/engine/SceneLayer.h>
#include <eagle/engine/components/Camera.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/CameraController.h>
#include <eagle/engine/components/DirectionalLight.h>
#include <eagle/engine/systems/RaytracerSystem.h>
#include <eagle/engine/systems/LightSystem.h>
#include <eagle/engine/systems/CameraSystem.h>
#include <eagle/engine/systems/physics/PhysicsSystem.h>
#include <eagle/engine/systems/physics/CollisionSystem.h>
#include <eagle/engine/components/SingletonComponent.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/physics/shapes/PlaneCollider.h>
#include <eagle/engine/components/physics/shapes/SphereCollider.h>
#include <eagle/engine/components/physics/shapes/BoxCollider.h>
#include <eagle/engine/systems/SpawnerSystem.h>
#include <eagle/engine/components/Spawner.h>
#include <eagle/engine/SceneManager.h>
#include <eagle/engine/systems/PaintOnCollisionSystem.h>

EG_ENGINE_BEGIN

SceneLayer::SceneLayer() {
    m_sceneRecreateCallback = [this](const OnSceneRecreate& ev){
        generate_playground();
    };

}

void SceneLayer::handle_attach() {

    EventMaster::instance().subscribe(&m_sceneRecreateCallback);

    Scene& scene = SceneManager::current_scene();

    auto& sceneData = SingletonComponent::get<SceneData>();
    auto& physics = SingletonComponent::get<PhysicsSettings>();
    physics.gravity = glm::vec3(0, -9.98f, 0);

    auto camera = scene.entities.create();
    camera.assign<Camera>();
    camera.assign<Transform>(glm::vec3(0, 10, -20), glm::quat(glm::vec3(0)), glm::vec3(1));
    camera.assign<CameraController>();
    camera.assign<Spawner>();

    auto light = scene.entities.create();
    light.assign<DirectionalLight>();
    light.assign<Transform>(glm::vec3(0), glm::quat(glm::radians(glm::vec3(-30, 0, 0))), glm::vec3(1))->hasChanged = true;

    generate_playground();

    scene.systems.add<CameraSystem>();
    scene.systems.add<SpawnerSystem>();
    scene.systems.add<PaintOnCollisionSystem>();
    scene.systems.add<LightSystem>();
    scene.systems.add<PhysicsSystem>();
    scene.systems.add<CollisionSystem>();
    scene.systems.add<RaytracerSystem>();
    scene.systems.configure();
}

void SceneLayer::handle_deattach() {
    EventMaster::instance().unsubscribe(&m_sceneRecreateCallback);
}

void SceneLayer::handle_update() {
    SceneManager::current_scene().systems.update_all(Time::delta_time());
}

void SceneLayer::handle_event(Event &e) {

}

void SceneLayer::generate_scene() {
    SceneData& sceneSettings = SingletonComponent::get<SceneData>();
    Scene& scene = SceneManager::current_scene();
    for (int i = 0; i < sceneSettings.maxSphereCount; i++) {
        auto e = scene.entities.create();
        auto sphere = e.assign<Sphere>();
        auto transform = e.assign<Transform>();
        e.assign<Rigidbody>();
        // Radius and radius
        sphere->radius = sceneSettings.sphereSizes.x + Random::value() * (sceneSettings.sphereSizes.y - sceneSettings.sphereSizes.x);
        glm::vec2 randomPos = random_inside_unit_circle() * sceneSettings.spherePlacementRadius;
        transform->set_position(glm::vec3(randomPos.x, sphere->radius, randomPos.y));
        // Reject spheres that are intersecting others
        bool skipSphere = false;
        for (auto other : scene.entities.entities_with_components<Sphere, Transform>()) {
            if (e == other) continue;

            auto otherSphere = other.component<Sphere>();
            auto otherTransform = other.component<Transform>();
            float minDist = sphere->radius + otherSphere->radius;
            if (glm::length2(transform->position() - otherTransform->position()) < minDist * minDist){
                e.destroy();
                skipSphere = true;
                break;
            }
        }
        if (skipSphere){
            continue;
        }
        // Albedo and specular color
        glm::vec3 color = glm::vec3(Random::value(), Random::value(), Random::value());
        bool metal = Random::value() < sceneSettings.metalPercent;
        sphere->albedo = metal ? glm::vec3(0.01f) : glm::vec3(color.r, color.g, color.b);
        sphere->specular = metal ? glm::vec3(color.r, color.g, color.b) : glm::vec3(0.01f);
        e.assign<Collider>(std::make_shared<SphereCollider>(sphere->radius));

    }
}

glm::vec2 SceneLayer::random_inside_unit_circle() {
    return glm::normalize(glm::vec2(Random::range(-1.0f, 1.0f), Random::range(-1.0f, 1.0f))) * Random::value();
}

void SceneLayer::generate_playground() {
    Scene& scene = SceneManager::current_scene();

    for (auto e : scene.entities.entities_with_components<Transform, Box>()){
        e.destroy();
    }
    for (auto e : scene.entities.entities_with_components<Transform, Sphere>()){
        e.destroy();
    }

    const glm::vec3 wallAlbedo = glm::vec3(0.32f);
    const glm::vec3 wallMetallic = glm::vec3(0.15f);

    auto plane = scene.entities.create();
    plane.assign<Transform>(glm::vec3(0), glm::quat(glm::vec3(0)), glm::vec3(1));
    plane.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    plane.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(25.0f, 1.0f, 25.0f)));
    plane.assign<Box>(glm::vec3(25.0f, 1.0f, 25.0f), wallAlbedo, wallMetallic);


    auto wall1 = scene.entities.create();
    wall1.assign<Transform>(glm::vec3(0, 2.5f, 26.0f), glm::quat(glm::vec3(0)), glm::vec3(1));
    wall1.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    wall1.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(25.0f, 3.5f, 1.0f)));
    wall1.assign<Box>(glm::vec3(25.0f, 3.5f, 1.0f), wallAlbedo, wallMetallic);

    auto wall2 = scene.entities.create();
    wall2.assign<Transform>(glm::vec3(0, 2.5f, -26.0f), glm::quat(glm::vec3(0)), glm::vec3(1));
    wall2.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    wall2.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(25.0f, 3.5f, 1.0f)));
    wall2.assign<Box>(glm::vec3(25.0f, 3.5f, 1.0f), wallAlbedo, wallMetallic);

    auto wall3 = scene.entities.create();
    wall3.assign<Transform>(glm::vec3(26.0f, 2.5f, 0), glm::quat(glm::vec3(0)), glm::vec3(1));
    wall3.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    wall3.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(1.0f, 3.5f, 25.0f)));
    wall3.assign<Box>(glm::vec3(1.0f, 3.5f, 25.0f), wallAlbedo, wallMetallic);

    auto wall4 = scene.entities.create();
    wall4.assign<Transform>(glm::vec3(-26.0f, 2.5f, 0), glm::quat(glm::vec3(0)), glm::vec3(1));
    wall4.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    wall4.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(1.0f, 3.5f, 25.0f)));
    wall4.assign<Box>(glm::vec3(1.0f, 3.5f, 25.0f), wallAlbedo, wallMetallic);

    auto platform1 = scene.entities.create();
    platform1.assign<Transform>(glm::vec3(-16.0f, 8.5f, 0), glm::quat(glm::vec3(120, 40, 0)), glm::vec3(1));
    platform1.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    platform1.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(2.0f, 1.0f, 6.0f)));
    platform1.assign<Box>(glm::vec3(2.0f, 1.0f, 6.0f), wallAlbedo, wallMetallic);

    auto platform2 = scene.entities.create();
    platform2.assign<Transform>(glm::vec3(16.0f, 8.5f, 0), glm::quat(glm::vec3(-120, 40, 20)), glm::vec3(1));
    platform2.assign<Rigidbody>(0.0f, 0.0f, 0.2f, 0.65f, Rigidbody::Mode::STATIC);
    platform2.assign<Collider>(std::make_shared<BoxCollider>(glm::vec3(2.0f, 1.0f, 6.0f)));
    platform2.assign<Box>(glm::vec3(2.0f, 1.0f, 6.0f), wallAlbedo, wallMetallic);
}

EG_ENGINE_END


