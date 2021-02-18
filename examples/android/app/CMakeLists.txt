cmake_minimum_required(VERSION 3.18.4)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(eagle_android SHARED
    src/main/cpp/main.cpp
    )

add_subdirectory(../../../ ${CMAKE_CURRENT_BINARY_DIR}/eagle)#kek


target_link_libraries(eagle_android
        PUBLIC
        eagle
        )