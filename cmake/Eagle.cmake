function(add_module NAME SOURCES)
    create_module(${NAME}_s STATIC "${SOURCES}")
endfunction()

function(create_module NAME MODE SOURCES)
    message("Sources: ${SOURCES}")
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