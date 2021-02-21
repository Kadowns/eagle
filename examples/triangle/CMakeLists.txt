add_subdirectory(../../ ${CMAKE_BINARY_DIR}/eagle)

set(TRIANGLE_SOURCE
        TriangleApp.cpp
        )

add_library(${EG_APP_LIB_NAME} STATIC ${TRIANGLE_SOURCE})

define_file_basename_for_sources(${EG_APP_LIB_NAME})

target_include_directories(${EG_APP_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${EG_APP_LIB_NAME} PUBLIC eagle)

set(SHADERS
        data/color.frag
        data/color.vert
        )

foreach (SHADER ${SHADERS})
    get_filename_component(SHADER_FILENAME ${SHADER} NAME)
    set(SHADER_ABS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER})
    get_filename_component(OUTPUT_DIR ${SHADER_ABS} DIRECTORY)
    set(SHADER_OUTPUT ${OUTPUT_DIR}/${SHADER_FILENAME}.spv)
    set(TARGET_NAME compileShaders-${SHADER_FILENAME})
    add_custom_target(
            ${TARGET_NAME}
            COMMENT "Compiling ${SHADER_ABS} to SPIR_V (${SHADER_OUTPUT})"
            BYPRODUCTS ${SHADER_OUTPUT}
            COMMAND $ENV{VULKAN_SDK}/Bin/glslc.exe ${SHADER_ABS} -o ${SHADER_OUTPUT}

    )
    add_dependencies(${EG_APP_LIB_NAME} ${TARGET_NAME})
endforeach ()