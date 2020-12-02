add_library(stb
        STATIC
        stb/stb_image.h
        stb/stb_image.cpp
        )

target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})