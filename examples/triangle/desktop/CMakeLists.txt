cmake_minimum_required(VERSION 3.17)
project(triangle)

set(EG_APP_EXE_NAME triangle)
set(EG_APP_LIB_NAME trianglelib)

add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/${EG_APP_LIB_NAME})

add_executable(${EG_APP_EXE_NAME} main.cpp)

define_file_basename_for_sources(${EG_APP_EXE_NAME})

target_link_libraries(${EG_APP_EXE_NAME} PRIVATE ${EG_APP_LIB_NAME})

add_custom_command(TARGET ${EG_APP_EXE_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../data/ $<TARGET_FILE_DIR:${EG_APP_EXE_NAME}>/data/)