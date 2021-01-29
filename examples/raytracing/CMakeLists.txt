set(RAYTRACER_INCLUDE
        RaytracingApp.h
        eagle/engine/EngineGlobalDefinitions.h
        eagle/engine/Engine.h
        eagle/engine/renderer/RenderLayer.h
        eagle/engine/renderer/RenderMaster.h
        eagle/engine/renderer/TextureLoader.h
        eagle/engine/components/Camera.h
        eagle/engine/components/CameraController.h
        eagle/engine/components/Transform.h
        eagle/engine/components/SceneData.h
        eagle/engine/components/RaytracerData.h
        eagle/engine/components/DirectionalLight.h
        eagle/engine/systems/RaytracerSystem.h
        eagle/engine/systems/CameraSystem.h
        eagle/engine/systems/LightSystem.h
        eagle/engine/components/SingletonComponent.h
        eagle/engine/components/Sphere.h
        eagle/engine/SceneLayer.h
        eagle/engine/systems/physics/PhysicsSystem.h
        eagle/engine/components/physics/Rigidbody.h
        eagle/engine/components/Box.h
        eagle/engine/systems/physics/CollisionSystem.h
        eagle/engine/components/physics/PhysicsSettings.h
        eagle/engine/components/physics/Collider.h
        eagle/engine/components/physics/shapes/BoxCollider.h
        eagle/engine/components/physics/shapes/ColliderShape.h
        eagle/engine/components/physics/shapes/SphereCollider.h
        eagle/engine/systems/physics/collision/CollisionManager.h
        eagle/engine/systems/physics/collision/Collision.h
        eagle/engine/components/physics/shapes/PlaneCollider.h
        eagle/engine/components/physics/CollisionSettings.h
        eagle/engine/systems/SpawnerSystem.h
        eagle/engine/components/Spawner.h
        eagle/engine/SceneManager.h
        eagle/engine/systems/PaintOnCollisionSystem.h
        eagle/editor/EditorMaster.h
        eagle/editor/EditorLayer.h
        eagle/editor/window/EditorWindow.h
        eagle/editor/window/ComputeDataEditor.h
        eagle/editor/window/EditorDockingWindow.h
        eagle/editor/window/SceneWindow.h
        eagle/editor/window/DebugSettingsWindow.h
        eagle/editor/EditorGlobalDefinitions.h
        eagle/editor/Editor.h
        )

set(RAYTRACER_SOURCE
        RaytracingApp.cpp
        eagle/engine/source/renderer/RenderLayer.cpp
        eagle/engine/source/renderer/RenderMaster.cpp
        eagle/engine/source/renderer/TextureLoader.cpp
        eagle/engine/source/systems/RaytracerSystem.cpp
        eagle/engine/source/systems/CameraSystem.cpp
        eagle/engine/source/systems/LightSystem.cpp
        eagle/engine/source/SceneLayer.cpp
        eagle/engine/source/systems/physics/PhysicsSystem.cpp
        eagle/engine/source/systems/physics/collision/shapes/SphereCollider.cpp
        eagle/engine/source/systems/physics/collision/shapes/ColliderShape.cpp
        eagle/engine/source/systems/physics/collision/shapes/BoxCollider.cpp
        eagle/engine/source/systems/physics/collision/CollisionManager.cpp
        eagle/engine/source/systems/physics/collision/shapes/PlaneCollider.cpp
        eagle/engine/source/systems/physics/CollisionSystem.cpp
        eagle/engine/source/systems/SpawnerSystem.cpp
        eagle/editor/source/EditorLayer.cpp
        eagle/editor/source/EditorMaster.cpp
        eagle/editor/source/window/EditorWindow.cpp
        eagle/editor/source/window/ComputeDataEditor.cpp
        eagle/editor/source/window/EditorDockingWindow.cpp
        eagle/editor/source/window/SceneWindow.cpp
        eagle/editor/source/window/DebugSettingsWindow.cpp
        )

set(RAYTRACER_ALL ${RAYTRACER_INCLUDE} ${RAYTRACER_SOURCE})

add_executable(eagle_raytracing "${RAYTRACER_ALL}" main.cpp)

define_file_basename_for_sources(eagle_raytracing)

set_target_properties(
        eagle_raytracing
        PROPERTIES
        LINKER_LANGUAGE CXX
)

target_include_directories(eagle_raytracing PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(eagle_raytracing eagle imgui glm stb entityx)

add_custom_command(TARGET eagle_raytracing POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:eagle_raytracing>/data/)