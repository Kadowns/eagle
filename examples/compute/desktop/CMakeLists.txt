cmake_minimum_required(VERSION 3.17)
project(compute)

set(CMAKE_CXX_STANDARD 20)

set(EG_APP_EXE_NAME compute)
set(EG_APP_LIB_NAME computelib)

add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/${EG_APP_LIB_NAME})

add_executable(${EG_APP_EXE_NAME} main.cpp)

define_file_basename_for_sources(${EG_APP_EXE_NAME})

target_link_libraries(${EG_APP_EXE_NAME} PRIVATE ${EG_APP_LIB_NAME})

add_custom_target(
        copy_data_folder
        COMMENT "Copying data folder"
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../data/ ${CMAKE_CURRENT_BINARY_DIR}/
)

add_dependencies(copy_data_folder ${EG_APP_LIB_NAME})
add_dependencies(${EG_APP_EXE_NAME} copy_data_folder)