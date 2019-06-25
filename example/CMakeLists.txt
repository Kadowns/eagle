include(${CMAKE_MODULE_PATH}/Eagle.cmake)

project(Dummy)

set(DUMMY_INCLUDE
        src/SceneTest.h
        src/OverlayTest.h
        src/DummyEvents.h
        )

set(DUMMY_SOURCES
        src/dummy.cpp
        )

set(DUMMY_ALL ${DUMMY_INCLUDE} ${DUMMY_SOURCES})

add_executable(dummy ${DUMMY_ALL})
define_file_basename_for_sources(dummy)

target_link_libraries(dummy eagle)
