include(${CMAKE_MODULE_PATH}/Eagle.cmake)

find_package(GLFW REQUIRED)

set(CORE_INCLUDE
        core/Core.h
        core/Application.h
        core/Window.h
        )

set(CORE_SOURCE
        core/source/Application.cpp
        core/source/Window.cpp
        )

set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})

create_module(eagle_s STATIC "${CORE_ALL}")
target_link_libraries(eagle_s GLFW)