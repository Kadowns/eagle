cmake_minimum_required(VERSION 3.14)

project(Eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

add_subdirectory(src)
