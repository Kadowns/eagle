cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EG_EXTERNAL_PATH ${EG_ROOT_PATH}/external)


option(BUILD_EG_ENGINE "Build eagle engine library" ON)
option(BUILD_EG_EDITOR "Build eagle editor executable (requires engine)" ON)

if(MSVC)
add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif(MSVC)

find_package(GLFW REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLSLANG REQUIRED)

set(CORE_INCLUDE
        eagle/Eagle.h
        eagle/core/GlobalDefinitions.h
        eagle/core/Application.h
        eagle/core/Window.h
        eagle/core/Input.h
        eagle/core/WindowGLFW.h
        eagle/core/Log.h
        eagle/core/Layer.h
        eagle/core/LayerStack.h
        eagle/core/events/Event.h
        eagle/core/events/WindowEvents.h
        eagle/core/events/InputEvents.h
        eagle/core/events/KeyCodes.h
        eagle/core/InputLayer.h)

set(CORE_SOURCE
        eagle/core/source/Application.cpp
        eagle/core/source/Window.cpp
        eagle/core/source/WindowGLFW.cpp
        eagle/core/source/GlobalDefinitions.cpp
        eagle/core/source/Log.cpp
        eagle/core/source/LayerStack.cpp
        eagle/core/source/Input.cpp eagle/core/source/InputLayer.cpp)


set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        eagle/core/renderer/RenderingCore.h
        eagle/core/renderer/RenderingContext.h
        eagle/core/renderer/Shader.h
        eagle/core/renderer/VertexLayout.h
        eagle/core/renderer/VertexBuffer.h
        eagle/core/renderer/IndexBuffer.h
        eagle/core/renderer/UniformBuffer.h
        eagle/core/renderer/DescriptorSet.h
        eagle/core/renderer/Texture2D.h
        eagle/core/renderer/Image.h
        eagle/core/renderer/RenderTarget.h
        eagle/core/renderer/DescriptorItem.h
        eagle/core/renderer/DescriptorSetLayout.h
        eagle/core/renderer/vulkan/VulkanContext.h
        eagle/core/renderer/vulkan/VulkanCore.h
        eagle/core/renderer/vulkan/VulkanHelper.h
        eagle/core/renderer/vulkan/VulkanShader.h
        eagle/core/renderer/vulkan/VulkanShaderCompiler.h
        eagle/core/renderer/vulkan/VulkanBuffer.h
        eagle/core/renderer/vulkan/VulkanVertexBuffer.h
        eagle/core/renderer/vulkan/VulkanIndexBuffer.h
        eagle/core/renderer/vulkan/VulkanUniformBuffer.h
        eagle/core/renderer/vulkan/VulkanDescriptorSet.h
        eagle/core/renderer/vulkan/VulkanTexture2D.h
        eagle/core/renderer/vulkan/VulkanImage.h
        eagle/core/renderer/vulkan/VulkanRenderTarget.h
        eagle/core/renderer/vulkan/VulkanCommand.h
        eagle/core/renderer/vulkan/VulkanCommandList.h
        eagle/core/renderer/vulkan/spirv_reflect.h
        eagle/core/renderer/vulkan/VulkanDescriptorSetLayout.h
        eagle/core/renderer/vulkan/VulkanConversor.h eagle/core/renderer/CommandBuffer.h eagle/core/renderer/vulkan/VulkanCleaner.h)

set(RENDERER_SOURCE
        eagle/core/source/renderer/Shader.cpp
        eagle/core/source/renderer/UniformBuffer.cpp
        eagle/core/source/renderer/DescriptorSet.cpp
        eagle/core/source/renderer/VertexLayout.cpp
        eagle/core/source/renderer/vulkan/VulkanContext.cpp
        eagle/core/source/renderer/vulkan/VulkanHelper.cpp
        eagle/core/source/renderer/vulkan/VulkanCore.cpp
        eagle/core/source/renderer/vulkan/VulkanShader.cpp
        eagle/core/source/renderer/vulkan/VulkanShaderCompiler.cpp
        eagle/core/source/renderer/vulkan/VulkanBuffer.cpp
        eagle/core/source/renderer/vulkan/VulkanVertexBuffer.cpp
        eagle/core/source/renderer/vulkan/VulkanIndexBuffer.cpp
        eagle/core/source/renderer/vulkan/VulkanUniformBuffer.cpp
        eagle/core/source/renderer/vulkan/VulkanDescriptorSet.cpp
        eagle/core/source/renderer/vulkan/VulkanDescriptorSetLayout.cpp
        eagle/core/source/renderer/vulkan/VulkanTexture2D.cpp
        eagle/core/source/renderer/vulkan/VulkanRenderTarget.cpp
        eagle/core/source/renderer/vulkan/spirv_reflect.cpp
        eagle/core/source/renderer/vulkan/VulkanConversor.cpp eagle/core/renderer/vulkan/VulkanCommandBuffer.h eagle/core/source/renderer/vulkan/VulkanCommandBuffer.cpp eagle/core/source/renderer/vulkan/VulkanCleaner.cpp)

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL} eagle/core/Core.h eagle/engine/resources/SceneManager.h)

add_library(eagle_core STATIC "${EAGLE_ALL}")

target_include_directories(
        eagle_core
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
        eagle_core
        PROPERTIES
        LINKER_LANGUAGE CXX
)

include(cmake/Eagle.cmake)

define_file_basename_for_sources(eagle_core)

target_link_libraries(eagle_core GLFW spdlog Vulkan::Vulkan glslang OSDependent OGLCompiler HLSL SPIRV)
if (BUILD_EG_ENGINE)
    message(STATUS "Building eagle engine!")

    find_package(BoostSerialization REQUIRED)
    find_package(assimp REQUIRED)
    find_package(JSON REQUIRED)
    add_subdirectory(${EG_EXTERNAL_PATH}/stb ${CMAKE_BINARY_DIR}/stb)
    add_subdirectory(${EG_EXTERNAL_PATH}/imgui ${CMAKE_BINARY_DIR}/imgui)
    add_subdirectory(${EG_EXTERNAL_PATH}/glm ${CMAKE_BINARY_DIR}/glm)

    add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data/")

    set(EG_ENGINE_INCLUDE
            eagle/engine/GlobalDefs.h
            eagle/engine/Engine.h
            eagle/engine/Camera.h
            eagle/engine/resources/ResourcesPool.h
            eagle/engine/resources/Material.h
            eagle/engine/resources/Mesh.h
            eagle/engine/resources/Scene.h
            eagle/engine/resources/SceneManager.h
            eagle/engine/resources/Texture.h
            eagle/engine/EntityManager.h
            eagle/engine/components/Transform.h
            eagle/engine/components/Renderable.h
            eagle/engine/TriggerEvent.h
            eagle/engine/Object.h
            eagle/engine/resources/ShaderModule.h eagle/engine/Renderer.h eagle/engine/LightSettings.h eagle/engine/MainLayer.h eagle/engine/components/Lights.h eagle/engine/systems/System.h eagle/engine/geometry/Vertex.h eagle/engine/geometry/MeshFactory.h eagle/engine/resources/Model.h eagle/engine/ModelImporter.h)

    set(EG_ENGINE_SOURCES
            eagle/engine/source/resources/Material.cpp
            eagle/engine/source/resources/Mesh.cpp
            eagle/engine/source/resources/ResourcesPool.cpp
            eagle/engine/source/resources/Scene.cpp
            eagle/engine/source/resources/SceneManager.cpp
            eagle/engine/source/EntityManager.cpp
            eagle/engine/source/Object.cpp
            eagle/engine/source/resources/ShaderModule.cpp eagle/engine/source/Renderer.cpp eagle/engine/source/MainLayer.cpp eagle/engine/source/LightSettings.cpp eagle/engine/source/geometry/MeshFactory.cpp eagle/engine/source/ModelImporter.cpp eagle/engine/source/resources/Texture.cpp eagle/engine/source/resources/Model.cpp)

    set(EG_ENGINE_ALL ${EG_ENGINE_INCLUDE} ${EG_ENGINE_SOURCES})

    add_library(eagle_engine STATIC ${EG_ENGINE_ALL})

    define_file_basename_for_sources(eagle_engine)

    target_include_directories(eagle_engine
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )


    target_link_libraries(eagle_engine eagle_core glm boost_serialization boost_filesystem nlohmann_json::nlohmann_json stb ${assimp})
endif()


if (BUILD_EG_ENGINE AND BUILD_EG_EDITOR)

    message(STATUS "Building eagle editor")

    set(EG_EDITOR_HEADERS
            eagle/editor/Selection.h
            eagle/editor/EditorLayer.h
            eagle/engine/Serializer.h
            eagle/editor/ResourceLoader.h
            eagle/editor/EditorCore.h
            eagle/editor/Editor.h
            eagle/editor/windows/EditorWindow.h
            eagle/editor/windows/ObjectEditor.h
            eagle/editor/windows/HierarchyWindow.h
            eagle/editor/windows/InspectorWindow.h
            eagle/editor/windows/SceneWindow.h
            eagle/editor/windows/ResourcesWindow.h
            eagle/editor/windows/EntityEditor.h
            eagle/editor/windows/MaterialEditor.h
            eagle/editor/windows/InspectorConfig.h)


    set(EG_EDITOR_SOURCES
            eagle/editor/source/EditorLayer.cpp
            eagle/editor/source/Selection.cpp
            eagle/editor/source/windows/SceneWindow.cpp
            eagle/engine/source/Serializer.cpp
            eagle/editor/source/ResourceLoader.cpp
            eagle/editor/source/windows/EditorWindow.cpp
            eagle/editor/source/windows/InspectorWindow.cpp
            eagle/editor/source/windows/HierarchyWindow.cpp
            eagle/editor/source/windows/ResourcesWindow.cpp
            eagle/editor/source/windows/EntityEditor.cpp
            eagle/editor/source/windows/MaterialEditor.cpp
            )


    set(EG_EDITOR_ALL ${EG_EDITOR_HEADERS} ${EG_EDITOR_SOURCES})

    add_library(eagle_editor STATIC ${EG_EDITOR_ALL})

    define_file_basename_for_sources(eagle_editor)

    target_include_directories(eagle_editor
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            )

    target_link_libraries(eagle_editor eagle_engine imgui)

    add_executable(eagle eagle/EntryPoint.cpp eagle/Eagle.h)

    define_file_basename_for_sources(eagle)

    target_link_libraries(eagle eagle_editor)

endif()
