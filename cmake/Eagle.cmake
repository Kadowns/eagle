function(add_module NAME SOURCES)
    create_module(${NAME}_s STATIC "${SOURCES}")
endfunction()

function(create_module NAME MODE SOURCES)

    add_library(${NAME} ${MODE} ${SOURCES})
    target_include_directories(
            ${NAME}
            PUBLIC
            $<BUILD_INTERFACE:${CMAKE_HOME_DIRECTORY}/src/>
            $<INSTALL_INTERFACE:include>
    )

    set_target_properties(
            ${NAME}
            PROPERTIES
            LINKER_LANGUAGE CXX
    )
endfunction()

function(define_file_basename_for_sources targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    message("Sources: ${source_files}")
    foreach(sourcefile ${source_files})
        # Get source file's current list of compile definitions.
        get_property(defs SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(basename "${sourcefile}" NAME_WE)
        list(APPEND defs "FILE_BASENAME=\"${basename}\"")
        # Set the updated compile definitions on the source file.
        set_property(
                SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS ${defs})
    endforeach()
endfunction()