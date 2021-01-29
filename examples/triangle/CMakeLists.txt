project(eagle_triangle VERSION 1.0.0)

set(TRIANGLE_HEADER
        TriangleApp.h
        TriangleLayer.h
        )

set(TRIANGLE_SOURCE
        TriangleApp.cpp
        TriangleLayer.cpp
        )

set(TRIANGLE_ALL
        ${TRIANGLE_HEADER}
        ${TRIANGLE_SOURCE}
        )

add_executable(eagle_triangle ${TRIANGLE_ALL} main.cpp)

define_file_basename_for_sources(eagle_triangle)

target_link_libraries(eagle_triangle PRIVATE eagle)

add_custom_command(TARGET eagle_triangle PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:eagle_triangle>/data/)