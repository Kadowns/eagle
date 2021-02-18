//
// Created by Ricardo on 2/14/2021.
//

#include <eagle/renderer/vulkan/platform/desktop/VulkanContextGLFW.h>
#include <eagle/platform/desktop/WindowGLFW.h>
#include <GLFW/glfw3.h>

Eagle::VulkanContextGLFW::VulkanContextGLFW(Eagle::WindowGLFW *windowGlfw) {
    m_window = windowGlfw;
}

void Eagle::VulkanContextGLFW::create_surface() {
    EG_CORE_TRACE("Creating window surface!");
    VK_CALL_ASSERT(glfwCreateWindowSurface(m_instance, (GLFWwindow *) m_window->native_window(), nullptr, &m_surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
    EG_CORE_TRACE("Window surface created!");
}

std::vector<const char*> Eagle::VulkanContextGLFW::get_platform_extensions() {
    EG_CORE_TRACE("Getting required extensions!");
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}
