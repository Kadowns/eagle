set(IMGUI_HEADERS
        imgui.h
        imconfig.h
        imgui_internal.h
        imstb_rectpack.h
        imstb_textedit.h
        imstb_truetype.h
        )

set(IMGUI_SOURCE
        imgui.cpp
        imgui_draw.cpp
        imgui_demo.cpp
        imgui_widgets.cpp
        )

set(IMGUI_ALL ${IMGUI_HEADERS} ${IMGUI_SOURCE})

add_library(imgui STATIC ${IMGUI_ALL})

target_include_directories(imgui PUBLIC .)

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
