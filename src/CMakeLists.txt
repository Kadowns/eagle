include(${CMAKE_MODULE_PATH}/Eagle.cmake)

find_package(GLFW REQUIRED)

set(CORE_INCLUDE
        eagle/core/Core.h
        eagle/core/Application.h
        eagle/core/Window.h
        )

set(CORE_SOURCE
        eagle/core/source/Application.cpp
        eagle/core/source/Window.cpp
        )

set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})

create_module(eagle_s STATIC "${CORE_ALL}")
target_link_libraries(eagle_s GLFW)
add_subdirectory(dummy)