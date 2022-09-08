//
// Created by Ricardo on 2/14/2021.
//

#include <eagle/renderer/vulkan/platform/desktop/vulkan_render_context_glfw.h>
#include <eagle/platform/desktop/desktop_window_glfw.h>
#include <GLFW/glfw3.h>

namespace eagle {

std::vector<const char*> VulkanRenderContextGLFW::query_instance_extensions() {
    auto extensions = VulkanRenderContext::query_instance_extensions();
    extensions.emplace_back("VK_KHR_win32_surface");
    return extensions;
}

VulkanRenderContextGLFW::VulkanRenderContextGLFW(
        const RenderContextCreateInfo& createInfo,
        const VulkanRenderContextCreateInfo& vkCreateInfo) :
        VulkanRenderContext(createInfo, vkCreateInfo) {

}

void VulkanRenderContextGLFW::create_surface() {
    auto result = glfwCreateWindowSurface(m_instance, (GLFWwindow*) m_createInfo.window->native_window(), nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

}