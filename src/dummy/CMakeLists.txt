include(${CMAKE_MODULE_PATH}/Eagle.cmake)
add_library(dummy dummy.cpp)
target_link_libraries(dummy eagle_s)

define_file_basename_for_sources(dummy)
