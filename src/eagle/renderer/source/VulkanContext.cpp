//
// Created by Novak on 08/05/2019.
//
#include "eagle/renderer/VulkanContext.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>


_EAGLE_BEGIN


VulkanContext::VulkanContext(uint32_t width, uint32_t height) :
    RenderingContext(width, height){
    LOGD << "Created vulkan context!";
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::init(const std::string& windownName) {

    LOGV << "Initializing vulkan context!";
    if (!glfwInit()){
        return;
    }

    m_window = glfwCreateWindow(m_width, m_height, windownName.c_str(), nullptr, nullptr);

    if (!m_window){
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_window);
    LOGV << "Vulkan context initialized";
}

void VulkanContext::deinit() {
    glfwTerminate();
}

void VulkanContext::refresh() {
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

bool VulkanContext::should_terminate_context() {
    return glfwWindowShouldClose(m_window);
}

_EAGLE_END
