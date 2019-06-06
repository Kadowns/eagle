include(${CMAKE_MODULE_PATH}/Eagle.cmake)
include(${CMAKE_MODULE_PATH}/DefineFileNames.cmake)

find_package(GLFW REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(Vulkan REQUIRED)


set(CORE_INCLUDE
        core/Core.h
        core/Application.h
        core/Window.h
        core/WindowGLFW.h
        core/Log.h
        core/events/Event.h
        core/events/WindowEvents.h
        )

set(CORE_SOURCE
        core/source/Application.cpp
        core/source/Window.cpp
        core/source/WindowGLFW.cpp
        core/source/Core.cpp
        core/source/Log.cpp
        )


set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        renderer/RenderingContext.h
        renderer/vulkan/VulkanContext.h
        renderer/vulkan/VulkanCore.h
        renderer/vulkan/VulkanHelper.h
        renderer/vulkan/VulkanShader.h
        )

set(RENDERER_SOURCE
        renderer/source/vulkan/VulkanContext.cpp
        renderer/source/vulkan/VulkanHelper.cpp
        renderer/source/vulkan/VulkanCore.cpp
        renderer/source/vulkan/VulkanShader.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL})

create_module(eagle_s STATIC "${EAGLE_ALL}")

define_file_basename_for_sources(eagle_s)

target_link_libraries(eagle_s GLFW spdlog Vulkan::Vulkan)
