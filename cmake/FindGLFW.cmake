if (GLFW_FOUND)
    return()
endif ()

add_library(GLFW STATIC IMPORTED GLOBAL)
target_include_directories(GLFW
        INTERFACE
        ${CMAKE_HOME_DIRECTORY}/external/glfw/include
        )

message("Including ${CMAKE_HOME_DIRECTORY}/external/glfw/include!")
set_target_properties(GLFW PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glfw/lib-vc2015/glfw3.lib)
set(GLFW_FOUND True)