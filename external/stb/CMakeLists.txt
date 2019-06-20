message(STATUS "Including stb library")

set(STB_SOURCES stb_image.h)

add_library(stb STATIC ${STB_SOURCES})

target_include_directories(stb INTERFACE ${CMAKE_HOME_DIRECTORY}/external/stb)

set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)
