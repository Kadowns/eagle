//
// Created by Ricardo on 2/14/2021.
//
#include <eagle/renderer/vulkan/platform/android/VulkanContextAndroid.h>
#include <eagle/renderer/vulkan/platform/android/vulkan_wrapper.h>
#include <eagle/platform/android/AndroidWindow.h>
#include <vulkan/vulkan_android.h>

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
    vkCreateAndroidSurfaceKHR(m_instance, &surfaceCreateInfo, NULL, &m_surface);
}

std::vector<const char*> Eagle::VulkanContextAndroid::get_platform_extensions() {
    return {"VK_KHR_android_surface"};
}

void Eagle::VulkanContextAndroid::on_surface_created() {
    load_vulkan();
    create_instance();
    load_instance_functions(m_instance);
    create_debug_callback();
    bind_physical_device();
    create_logical_device();
    load_device_functions(m_device);

    create_surface();
    create_sync_objects();
    create_command_pool();
    create_swapchain();
    create_render_pass();
    create_framebuffers();
    allocate_command_buffers();
}

void Eagle::VulkanContextAndroid::on_surface_destroyed() {
    cleanup_swapchain();
    VK_CALL vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

void Eagle::VulkanContextAndroid::init() {
    EG_CORE_TRACE("Initializing vulkan context!");


}


