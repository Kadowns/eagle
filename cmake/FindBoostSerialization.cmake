add_library(boost_serialization STATIC IMPORTED GLOBAL)
target_include_directories(boost_serialization
        INTERFACE
        ${EG_EXTERNAL_PATH}/boost/include
        )

add_library(boost_filesystem STATIC IMPORTED GLOBAL)
target_include_directories(boost_filesystem
        INTERFACE
        ${EG_EXTERNAL_PATH}/boost/include
        )

if (WIN32)

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set_target_properties(boost_serialization PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/boost/lib-mingw-w64/libserialization.a)
        set_target_properties(boost_filesystem PROPERTIES IMPORTED_LOCATION ${EG_EXTERNAL_PATH}/boost/lib-mingw-w64/libfilesystem.a)
    else()
        message(ERROR "Please add boost support for your compiler")
    endif()
endif()