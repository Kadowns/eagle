//
// Created by Ricardo on 2/14/2021.
//

#include <eagle/renderer/vulkan/platform/desktop/VulkanContextGLFW.h>
#include <eagle/platform/desktop/WindowGLFW.h>
#include <GLFW/glfw3.h>

eagle::VulkanContextGLFW::VulkanContextGLFW(eagle::WindowGLFW *windowGlfw) {
    m_window = windowGlfw;
}

void eagle::VulkanContextGLFW::create_surface() {
    EG_CORE_TRACE("Creating window surface!");
    VK_CALL_ASSERT(glfwCreateWindowSurface(m_instance, (GLFWwindow *) m_window->native_window(), nullptr, &m_surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
    EG_CORE_TRACE("Window surface created!");
}

std::vector<const char*> eagle::VulkanContextGLFW::get_platform_extensions() {
    EG_CORE_TRACE("Getting required extensions!");
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}


void eagle::VulkanContextGLFW::init(EventBus* eventBus) {
    EG_CORE_TRACE("Initializing vulkan context!");

    m_eventBus = eventBus;

    m_listener.attach(eventBus);
    m_listener.subscribe<OnWindowResized>([this](const OnWindowResized& ev){
       m_windowResized = true;
       return false;
    });

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

    EG_CORE_TRACE("Vulkan ready!");
}
