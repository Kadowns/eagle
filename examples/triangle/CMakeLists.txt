project(eagle_triangle VERSION 1.0.0)

add_executable(eagle_triangle TriangleLayer.h TriangleLayer.cpp main.cpp)

define_file_basename_for_sources(eagle_triangle)

target_link_libraries(eagle_triangle PRIVATE eagle)

add_custom_command(TARGET eagle_triangle PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:eagle_triangle>/data/)