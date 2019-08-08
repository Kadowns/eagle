include(${CMAKE_MODULE_PATH}/Eagle.cmake)

project(EG_Engine)

add_subdirectory(${EG_EXTERNAL_PATH}/stb ${CMAKE_BINARY_DIR}/stb)
add_subdirectory(${EG_EXTERNAL_PATH}/imgui ${CMAKE_BINARY_DIR}/imgui)
add_subdirectory(${EG_EXTERNAL_PATH}/entityx ${CMAKE_BINARY_DIR}/entityx)
add_subdirectory(${EG_EXTERNAL_PATH}/glm ${CMAKE_BINARY_DIR}/glm)

add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data/")

set(EG_ENGINE_INCLUDE
        EngineCore.h
        SceneLayer.h
        EditorLayer.h
        graphics/Renderable.h
        graphics/Camera.h
        graphics/Material.h
        graphics/Mesh.h
        graphics/Transform.h
        graphics/Scene.h editor/EditorWindow.h editor/HierarchyWindow.h editor/InspectorWindow.h editor/SceneWindow.h source/editor/SceneWindow.cpp)

set(EG_ENGINE_SOURCES
        source/EngineCore.cpp
        source/SceneLayer.cpp
        source/EditorLayer.cpp
        source/graphics/Material.cpp
        source/graphics/Mesh.cpp
        EntryPoint.cpp
        source/graphics/Scene.cpp source/editor/EditorWindow.cpp source/editor/HierarchyWindow.cpp source/editor/InspectorWindow.cpp source/editor/Selection.cpp)

set(EG_ENGINE_ALL ${EG_ENGINE_INCLUDE} ${EG_ENGINE_SOURCES} editor/Selection.h)

add_executable(eg_engine ${EG_ENGINE_ALL})
define_file_basename_for_sources(eg_engine)


target_link_libraries(eg_engine eagle stb glm imgui entityx)
