set(ENTITYX_BUILD_SHARED OFF CACHE BOOL "Build entityx shared libs" FORCE)
add_subdirectory(external/entityx)
add_subdirectory(external/glm)
add_subdirectory(external/imgui)
add_subdirectory(external/stb)

add_subdirectory(triangle)
add_subdirectory(raytracing)