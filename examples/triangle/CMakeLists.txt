add_subdirectory(../../ ${CMAKE_BINARY_DIR}/eagle)

set(TRIANGLE_SOURCE
        TriangleApp.cpp
        )

add_library(${EG_APP_LIB_NAME} STATIC ${TRIANGLE_SOURCE})

define_file_basename_for_sources(${EG_APP_LIB_NAME})

target_include_directories(${EG_APP_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${EG_APP_LIB_NAME} PUBLIC eagle)