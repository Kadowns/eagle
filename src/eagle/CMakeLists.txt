include(${CMAKE_MODULE_PATH}/Eagle.cmake)
include(${CMAKE_MODULE_PATH}/pch.cmake)

find_package(GLFW REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PLOG REQUIRED)

set(CORE_INCLUDE
        core/Core.h
        core/Application.h
        core/Window.h
        )

set(CORE_SOURCE
        core/source/Application.cpp
        core/source/Window.cpp
        core/stdafx.cpp
        )

set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        renderer/RenderingContext.h
        renderer/VulkanContext.h
        )

set(RENDERER_SOURCE
        renderer/source/RenderingContext.cpp
        renderer/source/VulkanContext.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL})

create_module(eagle_s STATIC "${EAGLE_ALL}")
target_link_libraries(eagle_s GLFW plog Vulkan::Vulkan)

add_precompiled_header(eagle_s core/stdafx.h core/stdafx.cpp)
