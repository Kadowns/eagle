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
        eagle/core/InputGLFW.h
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
        eagle/core/source/InputGLFW.cpp
        eagle/core/source/Core.cpp
        eagle/core/source/Log.cpp
        eagle/core/source/LayerStack.cpp
        )


set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        eagle/renderer/RenderingCore.h
        eagle/renderer/RenderingContext.h
        eagle/renderer/Shader.h
        eagle/renderer/VertexLayout.h
        eagle/renderer/VertexBuffer.h
        eagle/renderer/IndexBuffer.h
        eagle/renderer/UniformBuffer.h
        eagle/renderer/DescriptorSet.h
        eagle/renderer/Texture2D.h
        eagle/renderer/Image.h
        eagle/renderer/RenderTarget.h
        eagle/renderer/DescriptorItem.h
        eagle/renderer/vulkan/VulkanContext.h
        eagle/renderer/vulkan/VulkanCore.h
        eagle/renderer/vulkan/VulkanHelper.h
        eagle/renderer/vulkan/VulkanShader.h
        eagle/renderer/vulkan/VulkanShaderCompiler.h
        eagle/renderer/vulkan/VulkanBuffer.h
        eagle/renderer/vulkan/VulkanVertexBuffer.h
        eagle/renderer/vulkan/VulkanIndexBuffer.h
        eagle/renderer/vulkan/VulkanUniformBuffer.h
        eagle/renderer/vulkan/VulkanDescriptorSet.h
        eagle/renderer/vulkan/VulkanTexture2D.h
        eagle/renderer/vulkan/VulkanImage.h
        eagle/renderer/vulkan/VulkanRenderTarget.h
        eagle/renderer/vulkan/VulkanCommand.h
        eagle/renderer/vulkan/VulkanCommandList.h
        eagle/renderer/vulkan/spirv_reflect.h
        )

set(RENDERER_SOURCE
        eagle/renderer/source/Shader.cpp
        eagle/renderer/source/UniformBuffer.cpp
        eagle/renderer/source/DescriptorSet.cpp
        eagle/renderer/source/VertexLayout.cpp
        eagle/renderer/source/vulkan/VulkanContext.cpp
        eagle/renderer/source/vulkan/VulkanHelper.cpp
        eagle/renderer/source/vulkan/VulkanCore.cpp
        eagle/renderer/source/vulkan/VulkanShader.cpp
        eagle/renderer/source/vulkan/VulkanShaderCompiler.cpp
        eagle/renderer/source/vulkan/VulkanBuffer.cpp
        eagle/renderer/source/vulkan/VulkanVertexBuffer.cpp
        eagle/renderer/source/vulkan/VulkanIndexBuffer.cpp
        eagle/renderer/source/vulkan/VulkanUniformBuffer.cpp
        eagle/renderer/source/vulkan/VulkanDescriptorSet.cpp
        eagle/renderer/source/vulkan/VulkanTexture2D.cpp
        eagle/renderer/source/vulkan/VulkanRenderTarget.cpp
        eagle/renderer/source/vulkan/spirv_reflect.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL} eagle/renderer/DescriptorSetLayout.h eagle/renderer/vulkan/VulkanDescriptorSetLayout.h eagle/renderer/source/vulkan/VulkanDescriptorSetLayout.cpp)

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
    add_subdirectory(eagle/engine)
endif()
