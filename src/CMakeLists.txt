add_subdirectory(eagle)
add_subdirectory(dummy)

add_executable(eagle_entry eagle/core/main.cpp)
target_link_libraries(eagle_entry dummy)
message("Executable created!")