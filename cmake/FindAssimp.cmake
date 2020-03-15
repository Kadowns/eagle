function(prepare_lib name)
    add_library(${name} STATIC IMPORTED GLOBAL)
    target_include_directories(${name}
            INTERFACE
            ${EG_EXTERNAL_PATH}/assimp/include
            )

endfunction(prepare_lib)


prepare_lib(libassimp)
prepare_lib(xml)
prepare_lib(minizip)
prepare_lib(openddl_parsed)
prepare_lib(poly2trid)
prepare_lib(polyclipping)
prepare_lib(zip)
prepare_lib(zlib)


if (WIN32)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set_target_properties(libassimp PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libassimpd.a)
        set_target_properties(xml PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libIrrXMLd.a)
        set_target_properties(minizip PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libminizipd.a)
        set_target_properties(openddl_parsed PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libopenddl_parserd.a)
        set_target_properties(poly2trid PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libpoly2trid.a)
        set_target_properties(polyclipping PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libpolyclippingd.a)
        set_target_properties(zip PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libzipd.a)
        set_target_properties(zlib PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/assimp/lib-mingw-w64/libzlibd.a)
    else()
        message(ERROR "Please add assimp support for your compiler")
    endif()
else()
    message(ERROR "Please add assimp support for your platform")
endif()
set(assimp
        libassimp
        xml
        minizip
        openddl_parsed
        poly2trid
        polyclipping
        zip
        zlib
        )