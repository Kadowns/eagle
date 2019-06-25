include(${CMAKE_MODULE_PATH}/Eagle.cmake)

find_package(GLFW REQUIRED)
find_package(SPDLOG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(GLSLANG REQUIRED)
find_package(STB REQUIRED)


set(CORE_INCLUDE
        core/Core.h
        core/Application.h
        core/Window.h
        core/WindowGLFW.h
        core/Log.h
        core/Layer.h
        core/LayerStack.h
        core/events/Event.h
        core/events/WindowEvents.h
        )

set(CORE_SOURCE
        core/source/Application.cpp
        core/source/Window.cpp
        core/source/WindowGLFW.cpp
        core/source/Core.cpp
        core/source/Log.cpp
        core/source/LayerStack.cpp
        )


set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        renderer/RenderingContext.h
        renderer/Shader.h
        renderer/VertexBuffer.h
        renderer/IndexBuffer.h
        renderer/UniformBuffer.h
        renderer/ShaderItemLayout.h
        renderer/DescriptorSet.h
        renderer/Texture2D.h
        renderer/Image.h
        renderer/RenderTarget.h
        renderer/vulkan/VulkanContext.h
        renderer/vulkan/VulkanCore.h
        renderer/vulkan/VulkanHelper.h
        renderer/vulkan/VulkanShader.h
        renderer/vulkan/VulkanShaderCompiler.h
        renderer/vulkan/VulkanBuffer.h
        renderer/vulkan/VulkanVertexBuffer.h
        renderer/vulkan/VulkanIndexBuffer.h
        renderer/vulkan/VulkanUniformBuffer.h
        renderer/vulkan/VulkanDescriptorSet.h
        renderer/vulkan/VulkanTexture2D.h
        renderer/vulkan/VulkanImage.h
        renderer/vulkan/VulkanRenderTarget.h
        renderer/vulkan/VulkanCommand.h
        renderer/vulkan/spirv_reflect.h
        )

set(RENDERER_SOURCE
        renderer/source/RenderingContext.cpp
        renderer/source/Shader.cpp
        renderer/source/ShaderItemLayout.cpp
        renderer/source/UniformBuffer.cpp
        renderer/source/DescriptorSet.cpp
        renderer/source/Texture2D.cpp
        renderer/source/vulkan/VulkanContext.cpp
        renderer/source/vulkan/VulkanHelper.cpp
        renderer/source/vulkan/VulkanCore.cpp
        renderer/source/vulkan/VulkanShader.cpp
        renderer/source/vulkan/VulkanShaderCompiler.cpp
        renderer/source/vulkan/VulkanBuffer.cpp
        renderer/source/vulkan/VulkanVertexBuffer.cpp
        renderer/source/vulkan/VulkanIndexBuffer.cpp
        renderer/source/vulkan/VulkanUniformBuffer.cpp
        renderer/source/vulkan/VulkanDescriptorSet.cpp
        renderer/source/vulkan/VulkanTexture2D.cpp
        renderer/source/vulkan/VulkanRenderTarget.cpp
        renderer/source/vulkan/spirv_reflect.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL Eagle.h ${CORE_ALL} ${RENDERER_ALL})

create_module(eagle_s STATIC "${EAGLE_ALL}")

define_file_basename_for_sources(eagle_s)

target_link_libraries(eagle_s GLFW spdlog Vulkan::Vulkan glm glslang SPIRV OSDependent OGLCompiler stb)
