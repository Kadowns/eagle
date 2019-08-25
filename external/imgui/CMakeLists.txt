set(IMGUI_HEADERS
        imgui/imgui.h
        imgui/imconfig.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/imgui_stdlib.h
        )

set(IMGUI_SOURCE
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_stdlib.cpp
        )

set(IMGUI_ALL ${IMGUI_HEADERS} ${IMGUI_SOURCE})

add_library(imgui STATIC ${IMGUI_ALL})

target_include_directories(imgui PUBLIC .)

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
