message(STATUS "Including stb library")

set(STB_SOURCES stb_image.h stb_image.cpp)

add_library(stb STATIC ${STB_SOURCES})

target_include_directories(stb PUBLIC .)

set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
