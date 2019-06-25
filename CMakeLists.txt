cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake)

add_definitions(-DPROJECT_ROOT="${CMAKE_HOME_DIRECTORY}/example/data/")

add_subdirectory(eagle)

find_package(glm REQUIRED)
find_package(GLFW REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLSLANG REQUIRED)
find_package(STB REQUIRED)

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

define_file_basename_for_sources(eagle)

target_link_libraries(eagle GLFW spdlog Vulkan::Vulkan glslang SPIRV OSDependent OGLCompiler stb glm)


add_subdirectory(example)