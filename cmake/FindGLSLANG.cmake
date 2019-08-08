#add_subdirectory($ENV{VULKAN_SDK}/glslang ${CMAKE_BINARY_DIR}/glslang)
#include_directories($ENV{VULKAN_SDK}/glslang)


message("Including glslang and SPIRV")

function(import_target NAME INCLUDE_DIRS)
add_library(${NAME} STATIC IMPORTED GLOBAL)
target_include_directories(${NAME}
        INTERFACE
        ${INCLUDE_DIRS}
        )
endfunction()

#glslang stuff
import_target(glslang ${CMAKE_HOME_DIRECTORY}/external/glslang/include)
import_target(OSDependent ${CMAKE_HOME_DIRECTORY}/external/glslang/include)
import_target(OGLCompiler ${CMAKE_HOME_DIRECTORY}/external/glslang/include)
import_target(HLSL ${CMAKE_HOME_DIRECTORY}/external/glslang/include)

#spirv stuff
import_target(SPIRV ${CMAKE_HOME_DIRECTORY}/external/glslang/include)
#import_target(SPIRV-Tools ${CMAKE_HOME_DIRECTORY}/external/glslang/spirv-tools/include)
#import_target(SPIRV-Tools-opt ${CMAKE_HOME_DIRECTORY}/external/glslang/spirv-tools/include)

if (WIN32)
        set_target_properties(glslang PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libglslangd.a)
        set_target_properties(OSDependent PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libOSDependentd.a)
        set_target_properties(OGLCompiler PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libOGLCompilerd.a)
        set_target_properties(HLSL PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libHLSLd.a)
        set_target_properties(SPIRV PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libSPIRVd.a)

        #set_target_properties(SPIRV-Tools PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libSPIRV-Toolsd.a)
        #set_target_properties(SPIRV-Tools-opt PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glslang/lib-mingw-w64/libSPIRV-Tools-optd.a)
        message("GLSLANG properties set!")
endif ()



