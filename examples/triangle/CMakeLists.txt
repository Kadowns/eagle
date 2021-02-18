project(eagle_triangle VERSION 1.0.0)


set(TRIANGLE_SOURCE
        TriangleApp.cpp
        )

add_executable(eagle_triangle ${TRIANGLE_SOURCE} main.cpp)

define_file_basename_for_sources(eagle_triangle)

target_link_libraries(eagle_triangle PRIVATE eagle)

add_custom_command(TARGET eagle_triangle PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:eagle_triangle>/data/)