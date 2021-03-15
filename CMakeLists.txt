cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EG_EXTERNAL_PATH ${EG_ROOT_PATH}/external)


option(BUILD_EG_ENGINE "Build eagle engine library" OFF)
option(BUILD_EG_EDITOR "Build eagle editor executable (requires engine)" OFF)

add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data")
if(MSVC)
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif(MSVC)

add_subdirectory(${EG_EXTERNAL_PATH}/glslang ${CMAKE_BINARY_DIR}/glslang)
add_subdirectory(${EG_EXTERNAL_PATH}/spdlog ${CMAKE_BINARY_DIR}/spdlog)

set(EAGLE_SOURCE
        eagle/Application.cpp
        eagle/LayerStack.cpp
        eagle/Input.cpp
        eagle/InputLayer.cpp
        eagle/Random.cpp
        eagle/Time.cpp
        eagle/FileSystem.cpp
        eagle/events/Event.cpp

        eagle/renderer/VertexLayout.cpp
        eagle/renderer/vulkan/VulkanContext.cpp
        eagle/renderer/vulkan/VulkanHelper.cpp
        eagle/renderer/vulkan/VulkanCore.cpp
        eagle/renderer/vulkan/VulkanShader.cpp
        eagle/renderer/vulkan/VulkanShaderCompiler.cpp
        eagle/renderer/vulkan/VulkanBuffer.cpp
        eagle/renderer/vulkan/VulkanVertexBuffer.cpp
        eagle/renderer/vulkan/VulkanIndexBuffer.cpp
        eagle/renderer/vulkan/VulkanUniformBuffer.cpp
        eagle/renderer/vulkan/VulkanDescriptorSet.cpp
        eagle/renderer/vulkan/VulkanDescriptorSetLayout.cpp
        eagle/renderer/vulkan/VulkanTexture.cpp
        eagle/renderer/vulkan/spirv_reflect.cpp
        eagle/renderer/vulkan/VulkanConverter.cpp
        eagle/renderer/vulkan/VulkanCommandBuffer.cpp
        eagle/renderer/vulkan/VulkanCleaner.cpp
        eagle/renderer/vulkan/VulkanComputeShader.cpp
        eagle/renderer/vulkan/VulkanShaderUtils.cpp
        eagle/renderer/vulkan/VulkanStorageBuffer.cpp
        eagle/renderer/vulkan/VulkanFramebuffer.cpp
        eagle/renderer/vulkan/VulkanRenderPass.cpp
        eagle/renderer/vulkan/VulkanImage.cpp

#        eagle/core/source/renderer/vulkan/platform/android/VulkanContextAndroid.cpp
        )


set(EAGLE_PLATFORM_LIBS)

if (WIN32)
    message(STATUS "Eagle -- Including win32 sources")

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES")

    set(GLFW_STANDALONE OFF)
    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
    find_package(Vulkan REQUIRED)

    list(APPEND EAGLE_SOURCE
            eagle/platform/desktop/DesktopWindowGLFW.cpp
            eagle/platform/desktop/DesktopApplication.cpp
            eagle/platform/desktop/DesktopFileSystem.cpp
            eagle/renderer/vulkan/platform/desktop/VulkanContextGLFW.cpp
            )

    list(APPEND EAGLE_PLATFORM_LIBS glfw Vulkan::Vulkan)

elseif(ANDROID)
    message(STATUS " Eagle - Including android sources")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")


    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
            )
    target_include_directories(native_app_glue PUBLIC
            ${ANDROID_NDK}/sources/android/native_app_glue
            )

    find_library(log-lib log)


    list(APPEND EAGLE_SOURCE
            eagle/platform/android/AndroidWindow.cpp
            eagle/platform/android/AndroidApplication.cpp
            eagle/platform/android/AndroidFileSystem.cpp
            eagle/renderer/vulkan/platform/android/VulkanContextAndroid.cpp
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



set(EAGLE_ALL ${EAGLE_SOURCE})

add_library(eagle STATIC "${EAGLE_ALL}")

target_include_directories(
        eagle
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
        eagle
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
)

include(cmake/Eagle.cmake)

define_file_basename_for_sources(eagle)

target_link_libraries(eagle PUBLIC spdlog glslang OSDependent OGLCompiler HLSL SPIRV ${EAGLE_PLATFORM_LIBS})

#add_subdirectory(examples)
