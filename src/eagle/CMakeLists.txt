include(${CMAKE_MODULE_PATH}/Eagle.cmake)
include(${CMAKE_MODULE_PATH}/pch.cmake)

find_package(GLFW REQUIRED)
find_package(PLOG REQUIRED)

set(CORE_INCLUDE
        core/Core.h
        core/Application.h
        core/Window.h
        core/stdafx.h
        )

set(CORE_SOURCE
        core/source/Application.cpp
        core/source/Window.cpp
        core/stdafx.cpp
        )

set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})



create_module(eagle_s STATIC "${CORE_ALL}")
target_link_libraries(eagle_s GLFW plog)

add_precompiled_header(eagle_s core/stdafx.h FORCEINCLUDE)
#add_compile_options(/Yc"stdafx.h")
