cmake_minimum_required(VERSION 3.14)
project(Eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake)

add_subdirectory(src)

#add_executable(Eagle src/dummy/main.cpp src/eagle/core/source/Application.cpp src/eagle/core/include/Application.h)