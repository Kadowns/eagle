if (GLFW_FOUND)
    return()
endif ()
set(GLFW_STANDALONE OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_DOCS OFF)
add_subdirectory(${CMAKE_HOME_DIRECTORY}/external/glfw ${CMAKE_BINARY_DIR}/glfw)

set(GLFW_FOUND True)