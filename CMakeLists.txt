cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EG_EXTERNAL_PATH ${EG_ROOT_PATH}/external)


option(BUILD_EG_ENGINE "Build eagle engine executable" ON)

if(MSVC)
add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif(MSVC)

find_package(GLFW REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLSLANG REQUIRED)

set(CORE_INCLUDE
        eagle/Eagle.h
        eagle/core/Core.h
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
        )

set(CORE_SOURCE
        eagle/core/source/Application.cpp
        eagle/core/source/Window.cpp
        eagle/core/source/WindowGLFW.cpp
        eagle/core/source/Core.cpp
        eagle/core/source/Log.cpp
        eagle/core/source/LayerStack.cpp
        eagle/core/source/Input.cpp)


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
        )

set(RENDERER_SOURCE
        eagle/core/renderer/source/Shader.cpp
        eagle/core/renderer/source/UniformBuffer.cpp
        eagle/core/renderer/source/DescriptorSet.cpp
        eagle/core/renderer/source/VertexLayout.cpp
        eagle/core/renderer/source/vulkan/VulkanContext.cpp
        eagle/core/renderer/source/vulkan/VulkanHelper.cpp
        eagle/core/renderer/source/vulkan/VulkanCore.cpp
        eagle/core/renderer/source/vulkan/VulkanShader.cpp
        eagle/core/renderer/source/vulkan/VulkanShaderCompiler.cpp
        eagle/core/renderer/source/vulkan/VulkanBuffer.cpp
        eagle/core/renderer/source/vulkan/VulkanVertexBuffer.cpp
        eagle/core/renderer/source/vulkan/VulkanIndexBuffer.cpp
        eagle/core/renderer/source/vulkan/VulkanUniformBuffer.cpp
        eagle/core/renderer/source/vulkan/VulkanDescriptorSet.cpp
        eagle/core/renderer/source/vulkan/VulkanDescriptorSetLayout.cpp
        eagle/core/renderer/source/vulkan/VulkanTexture2D.cpp
        eagle/core/renderer/source/vulkan/VulkanRenderTarget.cpp
        eagle/core/renderer/source/vulkan/spirv_reflect.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL})

add_library(eagle STATIC "${EAGLE_ALL}")

target_include_directories(
        eagle
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
        eagle
        PROPERTIES
        LINKER_LANGUAGE CXX
)

include(cmake/Eagle.cmake)

define_file_basename_for_sources(eagle)

target_link_libraries(eagle GLFW spdlog Vulkan::Vulkan glslang OSDependent OGLCompiler HLSL SPIRV)
if (BUILD_EG_ENGINE)
    message(STATUS "Building eagle engine!")

    project(EG_Engine)

    find_package(BoostSerialization REQUIRED)
    add_subdirectory(${EG_EXTERNAL_PATH}/stb ${CMAKE_BINARY_DIR}/stb)
    add_subdirectory(${EG_EXTERNAL_PATH}/imgui ${CMAKE_BINARY_DIR}/imgui)
    add_subdirectory(${EG_EXTERNAL_PATH}/entityx ${CMAKE_BINARY_DIR}/entityx)
    add_subdirectory(${EG_EXTERNAL_PATH}/glm ${CMAKE_BINARY_DIR}/glm)

    add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data")

    set(EG_ENGINE_INCLUDE
            eagle/engine/EngineCore.h
            eagle/engine/Camera.h
            eagle/engine/resources/ResourcesPool.h
            eagle/engine/resources/Material.h
            eagle/engine/resources/Mesh.h
            eagle/engine/components/Transform.h
            eagle/engine/components/Renderable.h
            eagle/engine/Scene.h
            eagle/editor/Serializer.h

            )

    set(EG_ENGINE_SOURCES
            eagle/engine/EntryPoint.cpp
            eagle/engine/source/resources/Material.cpp
            eagle/engine/source/resources/Mesh.cpp
            eagle/engine/source/resources/ResourcesPool.cpp
            eagle/engine/source/Scene.cpp
            eagle/engine/source/EngineCore.cpp
            eagle/editor/source/Serializer.cpp
            )

    set(EG_ENGINE_ALL ${EG_ENGINE_INCLUDE} ${EG_ENGINE_SOURCES} )

    set(EG_EDITOR_HEADERS
            eagle/editor/Selection.h
            eagle/editor/EditorLayer.h
            eagle/editor/EditorWindow.h
            eagle/editor/HierarchyWindow.h
            eagle/editor/InspectorWindow.h
            eagle/editor/SceneWindow.h
            eagle/editor/ResourcesWindow.h
            eagle/editor/ResourceLoader.h)


    set(EG_EDITOR_SOURCES
            eagle/editor/source/EditorLayer.cpp
            eagle/editor/source/EditorWindow.cpp
            eagle/editor/source/HierarchyWindow.cpp
            eagle/editor/source/InspectorWindow.cpp
            eagle/editor/source/Selection.cpp
            eagle/editor/source/ResourcesWindow.cpp
            eagle/editor/source/SceneWindow.cpp
            )

    set(EG_EDITOR_ALL ${EG_EDITOR_HEADERS} ${EG_EDITOR_SOURCES})


    add_executable(eg_engine ${EG_ENGINE_ALL} ${EG_EDITOR_ALL})
    define_file_basename_for_sources(eg_engine)


    target_link_libraries(eg_engine eagle stb glm imgui entityx boost_serialization boost_filesystem)



endif()
