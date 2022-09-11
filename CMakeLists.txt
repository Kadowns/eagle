cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EG_EXTERNAL_PATH ${EG_ROOT_PATH}/external)

add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data")
if(MSVC)
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif(MSVC)

add_subdirectory(${EG_EXTERNAL_PATH}/spdlog ${CMAKE_BINARY_DIR}/spdlog)

set(EAGLE_HEADER
        eagle/application.h
        eagle/application_delegate.h
        eagle/core_global_definitions.h
        eagle/eagle.h
        eagle/file_system.h
        eagle/input.h
        eagle/log.h
        eagle/random.h
        eagle/timer.h
        eagle/window.h

        eagle/memory/buffer.h
        eagle/memory/pointer.h
        eagle/memory/pool_allocator.h
        eagle/memory/stack_allocator.h

        eagle/events/event.h
        eagle/events/input_events.h
        eagle/events/key_codes.h
        eagle/events/window_events.h

        eagle/renderer/command_buffer.h
        eagle/renderer/command_queue.h
        eagle/renderer/compute_shader.h
        eagle/renderer/descriptor_item.h
        eagle/renderer/descriptor_set.h
        eagle/renderer/descriptor_set_layout.h
        eagle/renderer/framebuffer.h
        eagle/renderer/gpu_buffer.h
        eagle/renderer/image.h
        eagle/renderer/index_buffer.h
        eagle/renderer/render_pass.h
        eagle/renderer/render_target.h
        eagle/renderer/renderer_global_definitions.h
        eagle/renderer/rendering_context.h
        eagle/renderer/render_context.h
        eagle/renderer/semaphore.h
        eagle/renderer/shader.h
        eagle/renderer/storage_buffer.h
        eagle/renderer/texture.h
        eagle/renderer/uniform_buffer.h
        eagle/renderer/vertex_buffer.h
        eagle/renderer/vertex_layout.h

        eagle/renderer/vulkan/spirv_reflect.h
        eagle/renderer/vulkan/vulkan_buffer.h
        eagle/renderer/vulkan/vulkan_cleaner.h
        eagle/renderer/vulkan/vulkan_command_buffer.h
        eagle/renderer/vulkan/vulkan_command_queue.h
        eagle/renderer/vulkan/vulkan_converter.h
        eagle/renderer/vulkan/vulkan_deleter.h
        eagle/renderer/vulkan/vulkan_descriptor_set.h
        eagle/renderer/vulkan/vulkan_descriptor_set_layout.h
        eagle/renderer/vulkan/vulkan_exception.h
        eagle/renderer/vulkan/vulkan_fence.h
        eagle/renderer/vulkan/vulkan_framebuffer.h
        eagle/renderer/vulkan/vulkan_global_definitions.h
        eagle/renderer/vulkan/vulkan_helper.h
        eagle/renderer/vulkan/vulkan_image.h
        eagle/renderer/vulkan/vulkan_index_buffer.h
        eagle/renderer/vulkan/vulkan_render_context.h
        eagle/renderer/vulkan/vulkan_render_pass.h
        eagle/renderer/vulkan/vulkan_semaphore.h
        eagle/renderer/vulkan/vulkan_shader.h
        eagle/renderer/vulkan/vulkan_shader_utils.h
        eagle/renderer/vulkan/vulkan_storage_buffer.h
        eagle/renderer/vulkan/vulkan_texture.h
        eagle/renderer/vulkan/vulkan_uniform_buffer.h
        eagle/renderer/vulkan/vulkan_vertex_buffer.h
        )

set(EAGLE_SOURCE
        eagle/application.cpp
        eagle/input.cpp
        eagle/random.cpp
        eagle/timer.cpp
        eagle/file_system.cpp
        eagle/window.cpp
        eagle/memory/buffer.cpp
        eagle/events/event.cpp

        eagle/renderer/command_queue.cpp
        eagle/renderer/command_buffer.cpp
        eagle/renderer/vertex_layout.cpp
        eagle/renderer/render_context.cpp
        eagle/renderer/shader.cpp

        eagle/renderer/vulkan/spirv_reflect.cpp
        eagle/renderer/vulkan/vulkan_buffer.cpp
        eagle/renderer/vulkan/vulkan_cleaner.cpp
        eagle/renderer/vulkan/vulkan_command_buffer.cpp
        eagle/renderer/vulkan/vulkan_command_queue.cpp
        eagle/renderer/vulkan/vulkan_converter.cpp
        eagle/renderer/vulkan/vulkan_deleter.cpp
        eagle/renderer/vulkan/vulkan_descriptor_set.cpp
        eagle/renderer/vulkan/vulkan_descriptor_set_layout.cpp
        eagle/renderer/vulkan/vulkan_exception.cpp
        eagle/renderer/vulkan/vulkan_fence.cpp
        eagle/renderer/vulkan/vulkan_framebuffer.cpp
        eagle/renderer/vulkan/vulkan_global_definitions.cpp
        eagle/renderer/vulkan/vulkan_helper.cpp
        eagle/renderer/vulkan/vulkan_image.cpp
        eagle/renderer/vulkan/vulkan_index_buffer.cpp
        eagle/renderer/vulkan/vulkan_render_context.cpp
        eagle/renderer/vulkan/vulkan_render_pass.cpp
        eagle/renderer/vulkan/vulkan_semaphore.cpp
        eagle/renderer/vulkan/vulkan_shader.cpp
        eagle/renderer/vulkan/vulkan_shader_utils.cpp
        eagle/renderer/vulkan/vulkan_storage_buffer.cpp
        eagle/renderer/vulkan/vulkan_texture.cpp
        eagle/renderer/vulkan/vulkan_uniform_buffer.cpp
        eagle/renderer/vulkan/vulkan_vertex_buffer.cpp
        )


set(EAGLE_PLATFORM_LIBS)

if (WIN32)
    message(STATUS "Eagle -- Including win32 sources")

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES")

    set(EG_PLATFORM EG_PLATFORM_WIN32)

    set(GLFW_STANDALONE OFF)
    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
    find_package(Vulkan REQUIRED)

    list(APPEND EAGLE_HEADER
            eagle/platform/desktop/desktop_window_glfw.h
            eagle/platform/desktop/desktop_application.h
            eagle/platform/desktop/desktop_file_system.h
            eagle/renderer/vulkan/platform/desktop/vulkan_render_context_glfw.h
            )

    list(APPEND EAGLE_SOURCE
            eagle/platform/desktop/desktop_window_glfw.cpp
            eagle/platform/desktop/desktop_application.cpp
            eagle/platform/desktop/desktop_file_system.cpp
            eagle/renderer/vulkan/platform/desktop/vulkan_render_context_glfw.cpp
            )

    list(APPEND EAGLE_PLATFORM_LIBS glfw Vulkan::Vulkan)

elseif(ANDROID)
    message(STATUS " Eagle - Including android sources")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")

    set(EG_PLATFORM EG_PLATFORM_ANDROID)


    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
            )
    target_include_directories(native_app_glue PUBLIC
            ${ANDROID_NDK}/sources/android/native_app_glue
            )

    find_library(log-lib log)

    list(APPEND EAGLE_HEADER
            eagle/platform/android/android_window.h
            eagle/platform/android/android_application.h
            eagle/platform/android/android_file_system.h
            eagle/renderer/vulkan/platform/android/vulkan_context_android.h
            eagle/renderer/vulkan/platform/android/vulkan_wrapper.h
            )

    list(APPEND EAGLE_SOURCE
            eagle/platform/android/android_window.cpp
            eagle/platform/android/android_application.cpp
            eagle/platform/android/android_file_system.cpp
            eagle/renderer/vulkan/platform/android/vulkan_context_android.cpp
            eagle/renderer/vulkan/platform/android/vulkan_wrapper.cpp
            )


    list(APPEND EAGLE_PLATFORM_LIBS
            android
            native_app_glue
            ${log-lib}
            )
else()
    message(ERROR "Unsuported platform!")
endif()

set(EAGLE_ALL ${EAGLE_HEADER} ${EAGLE_SOURCE})

add_library(eagle STATIC "${EAGLE_ALL}")

target_include_directories(
        eagle
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(eagle PUBLIC ${EG_PLATFORM})

set_target_properties(
        eagle
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 20
)

set_property(TARGET eagle PROPERTY CXX_STANDARD 20)

include(cmake/Eagle.cmake)

define_file_basename_for_sources(eagle)

target_link_libraries(eagle PUBLIC spdlog ${EAGLE_PLATFORM_LIBS})

