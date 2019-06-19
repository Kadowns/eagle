include(${CMAKE_MODULE_PATH}/Eagle.cmake)

set(DUMMY_INCLUDE
        SceneTest.h
        OverlayTest.h
        DummyEvents.h
        )

set(DUMMY_SOURCES
        dummy.cpp
        )

set(DUMMY_ALL ${DUMMY_INCLUDE} ${DUMMY_SOURCES})

add_executable(dummy ${DUMMY_ALL})
define_file_basename_for_sources(dummy)

target_link_libraries(dummy eagle_s)
