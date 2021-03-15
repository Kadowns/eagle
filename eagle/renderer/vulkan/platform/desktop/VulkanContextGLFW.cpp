//
// Created by Ricardo on 2/14/2021.
//

#include <eagle/renderer/vulkan/platform/desktop/VulkanContextGLFW.h>
#include <eagle/platform/desktop/DesktopWindowGLFW.h>
#include <GLFW/glfw3.h>

eagle::VulkanContextGLFW::VulkanContextGLFW(eagle::DesktopWindowGLFW *windowGlfw) {
    m_window = windowGlfw;
}

void eagle::VulkanContextGLFW::create_surface() {
    EG_TRACE("eagle","Creating window surface!");
    VK_CALL_ASSERT(glfwCreateWindowSurface(m_instance, (GLFWwindow *) m_window->native_window(), nullptr, &m_surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
    EG_TRACE("eagle","Window surface created!");
}

std::vector<const char*> eagle::VulkanContextGLFW::get_platform_extensions() {
    EG_TRACE("eagle","Getting required extensions!");
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}


void eagle::VulkanContextGLFW::init() {
    EG_TRACE("eagle","Initializing vulkan context!");

    create_instance();
    create_debug_callback();
    create_surface();
    bind_physical_device();
    create_logical_device();
    create_sync_objects();
    create_command_pool();

    create_swapchain();
    create_render_pass();
    create_framebuffers();

    EG_TRACE("eagle","Vulkan ready!");
}
