//
// Created by Novak on 17/03/2020.
//

#ifndef EAGLE_ENGINE_H
#define EAGLE_ENGINE_H

#include <eagle/engine/components/Box.h>
#include <eagle/engine/components/Camera.h>
#include <eagle/engine/components/CameraController.h>
#include <eagle/engine/components/DirectionalLight.h>
#include <eagle/engine/components/RaytracerData.h>
#include <eagle/engine/components/SceneData.h>
#include <eagle/engine/components/SingletonComponent.h>
#include <eagle/engine/components/Spawner.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/Transform.h>

#include <eagle/engine/components/physics/Collider.h>
#include <eagle/engine/components/physics/CollisionSettings.h>
#include <eagle/engine/components/physics/PhysicsSettings.h>
#include <eagle/engine/components/physics/Rigidbody.h>
#include <eagle/engine/components/physics/shapes/BoxCollider.h>
#include <eagle/engine/components/physics/shapes/ColliderShape.h>
#include <eagle/engine/components/physics/shapes/PlaneCollider.h>
#include <eagle/engine/components/physics/shapes/SphereCollider.h>

#include <eagle/engine/renderer/RenderLayer.h>
#include <eagle/engine/renderer/RenderMaster.h>
#include <eagle/engine/renderer/TextureLoader.h>

#include <eagle/engine/EventMaster.h>
#include <eagle/engine/SceneLayer.h>
#include <eagle/engine/SceneManager.h>

#endif //EAGLE_ENGINE_H
