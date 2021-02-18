//
// Created by Ricardo on 2/14/2021.
//
#include <eagle/renderer/vulkan/platform/android/VulkanContextAndroid.h>
#include <eagle/renderer/vulkan/platform/android/vulkan_wrapper.h>
#include <eagle/platform/android/AndroidWindow.h>
#include <vulkan/vulkan_android.h>


Eagle::VulkanContextAndroid::VulkanContextAndroid() {

}

Eagle::VulkanContextAndroid::~VulkanContextAndroid() {
    close_vulkan();
}

void Eagle::VulkanContextAndroid::create_surface() {
    VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.window = (ANativeWindow)m_window->native_window();
    vkCreateAndroidSurfaceKHR(m_instance, &surfaceCreateInfo, NULL, &m_surface);
}

std::vector<const char*> Eagle::VulkanContextAndroid::get_platform_extensions() {
    return {"VK_KHR_android_surface"};
}

Eagle::VulkanContextAndroid::VulkanContextAndroid(Eagle::AndroidWindow *window) {
    open_vulkan();
    m_window = window;
}

void Eagle::VulkanContextAndroid::on_surface_created() {
    if (m_surface == VK_NULL_HANDLE){
        create_surface();
    }


}

void Eagle::VulkanContextAndroid::on_surface_destroyed() {

}


