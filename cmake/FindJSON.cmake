message(STATUS "including json lib")
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(${EG_EXTERNAL_PATH}/json ${CMAKE_BINARY_DIR}/json)