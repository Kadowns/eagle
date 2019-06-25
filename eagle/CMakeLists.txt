include(${CMAKE_MODULE_PATH}/Eagle.cmake)

set(CORE_INCLUDE
        eagle/Eagle.h
        eagle/core/Core.h
        eagle/core/Application.h
        eagle/core/Window.h
        eagle/core/WindowGLFW.h
        eagle/core/Log.h
        eagle/core/Layer.h
        eagle/core/LayerStack.h
        eagle/core/events/Event.h
        eagle/core/events/WindowEvents.h
        )

set(CORE_SOURCE
        eagle/core/source/Application.cpp
        eagle/core/source/Window.cpp
        eagle/core/source/WindowGLFW.cpp
        eagle/core/source/Core.cpp
        eagle/core/source/Log.cpp
        eagle/core/source/LayerStack.cpp
        )


set(CORE_ALL ${CORE_INCLUDE} ${CORE_SOURCE})


set(RENDERER_INCLUDE
        eagle/renderer/RenderingContext.h
        eagle/renderer/Shader.h
        eagle/renderer/VertexBuffer.h
        eagle/renderer/IndexBuffer.h
        eagle/renderer/UniformBuffer.h
        eagle/renderer/ShaderItemLayout.h
        eagle/renderer/DescriptorSet.h
        eagle/renderer/Texture2D.h
        eagle/renderer/Image.h
        eagle/renderer/RenderTarget.h
        eagle/renderer/vulkan/VulkanContext.h
        eagle/renderer/vulkan/VulkanCore.h
        eagle/renderer/vulkan/VulkanHelper.h
        eagle/renderer/vulkan/VulkanShader.h
        eagle/renderer/vulkan/VulkanShaderCompiler.h
        eagle/renderer/vulkan/VulkanBuffer.h
        eagle/renderer/vulkan/VulkanVertexBuffer.h
        eagle/renderer/vulkan/VulkanIndexBuffer.h
        eagle/renderer/vulkan/VulkanUniformBuffer.h
        eagle/renderer/vulkan/VulkanDescriptorSet.h
        eagle/renderer/vulkan/VulkanTexture2D.h
        eagle/renderer/vulkan/VulkanImage.h
        eagle/renderer/vulkan/VulkanRenderTarget.h
        eagle/renderer/vulkan/VulkanCommand.h
        eagle/renderer/vulkan/spirv_reflect.h
        )

set(RENDERER_SOURCE
        eagle/renderer/source/RenderingContext.cpp
        eagle/renderer/source/Shader.cpp
        eagle/renderer/source/ShaderItemLayout.cpp
        eagle/renderer/source/UniformBuffer.cpp
        eagle/renderer/source/DescriptorSet.cpp
        eagle/renderer/source/Texture2D.cpp
        eagle/renderer/source/vulkan/VulkanContext.cpp
        eagle/renderer/source/vulkan/VulkanHelper.cpp
        eagle/renderer/source/vulkan/VulkanCore.cpp
        eagle/renderer/source/vulkan/VulkanShader.cpp
        eagle/renderer/source/vulkan/VulkanShaderCompiler.cpp
        eagle/renderer/source/vulkan/VulkanBuffer.cpp
        eagle/renderer/source/vulkan/VulkanVertexBuffer.cpp
        eagle/renderer/source/vulkan/VulkanIndexBuffer.cpp
        eagle/renderer/source/vulkan/VulkanUniformBuffer.cpp
        eagle/renderer/source/vulkan/VulkanDescriptorSet.cpp
        eagle/renderer/source/vulkan/VulkanTexture2D.cpp
        eagle/renderer/source/vulkan/VulkanRenderTarget.cpp
        eagle/renderer/source/vulkan/spirv_reflect.cpp
        )

set(RENDERER_ALL ${RENDERER_INCLUDE} ${RENDERER_SOURCE})

set(EAGLE_ALL ${CORE_ALL} ${RENDERER_ALL} PARENT_SCOPE)