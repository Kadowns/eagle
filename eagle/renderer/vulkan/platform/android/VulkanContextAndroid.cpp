//
// Created by Ricardo on 2/14/2021.
//
#include <eagle/renderer/vulkan/platform/android/VulkanContextAndroid.h>
#include <eagle/renderer/vulkan/platform/android/vulkan_wrapper.h>
#include <eagle/platform/android/AndroidWindow.h>
#include <vulkan/vulkan_android.h>
#include <eagle/renderer/vulkan/VulkanContext.h>

Eagle::VulkanContextAndroid::VulkanContextAndroid(Eagle::AndroidWindow *window) {
    m_window = window;
}

Eagle::VulkanContextAndroid::~VulkanContextAndroid() {
    close_vulkan();
}

void Eagle::VulkanContextAndroid::create_surface() {
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.window = (ANativeWindow*)m_window->native_window();
    vkCreateAndroidSurfaceKHR(m_instance, &surfaceCreateInfo, nullptr, &m_surface);
}

std::vector<const char*> Eagle::VulkanContextAndroid::get_platform_extensions() {
    return {VK_KHR_ANDROID_SURFACE_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME};
}

void Eagle::VulkanContextAndroid::recreate_surface() {
    VK_CALL vkDeviceWaitIdle(m_device);

    create_surface();
    create_swapchain();
    create_framebuffers();
    create_command_pool();
    allocate_command_buffers();

    recreate_objects();

    EG_CORE_TRACE("Swapchain recreated!");
}

void Eagle::VulkanContextAndroid::destroy_surface() {
    VK_CALL vkDeviceWaitIdle(m_device);

    clear_objects();

    m_present.framebuffer.reset();
    m_commandBuffers.clear();

    VK_CALL vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);

    VK_CALL vkDestroyCommandPool(m_device, m_computeCommandPool, nullptr);

    VK_CALL vkDestroySwapchainKHR(m_device, m_present.swapchain, nullptr);

    VK_CALL vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

void Eagle::VulkanContextAndroid::init() {
    EG_CORE_TRACE("Initializing vulkan context!");
    if (m_initialized){
        return;
    }
    load_vulkan();
    create_instance();
    load_instance_functions(m_instance);
    create_debug_callback();
    bind_physical_device();
    create_logical_device();
    load_device_functions(m_device);
    create_sync_objects();

    create_surface();
    create_command_pool();
    create_swapchain();
    create_render_pass();
    create_framebuffers();
    allocate_command_buffers();

    m_initialized = true;
}


