//
// Created by Novak on 08/05/2019.
//



#include "eagle/renderer/vulkan/VulkanContext.h"
#include "eagle/core/Core.h"

#include <GLFW/glfw3.h>

_EAGLE_BEGIN

bool VulkanContext::enableValidationLayers = true;

VulkanContext::VulkanContext(uint32_t width, uint32_t height) :
    RenderingContext(width, height){
    EAGLE_SET_INFO(EAGLE_RENDERING_CONTEXT, "Vulkan");
    LOGD << "Created vulkan context!";
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::init() {

    LOGV << "Initializing vulkan context!";
    create_instance();
    create_debug_callback();
    LOGV << "Vulkan context initialized";
}

void VulkanContext::deinit() {

}

void VulkanContext::refresh() {

}

void VulkanContext::create_instance() {

    if (enableValidationLayers) {
        if (!validation_layers_supported())
            throw std::runtime_error("validation layers requested, but not available!");

    }

    //Informa��es pra vulkan fazer umas otimiza��es
    ///Window Info-----------------------------------------------
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = EAGLE_GET_INFO(EAGLE_APP_NAME).c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "Eagle";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    ///---------------------------------------------------------------


    ///Create Info-----------------------------------------------------
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = get_required_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    //passa pro struct as validation layers (serve pra testar se a gente ta fazendo merda no c�digo)
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    ///---------------------------------------------------------------

    if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vkInstance!");
    }

}

bool VulkanContext::validation_layers_supported() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanContext::get_required_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void VulkanContext::create_debug_callback() {
    if (!enableValidationLayers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    //Qu�o importante a mensagem precisa ser para ser enviada
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    //Quais tipos de mensagem s�o enviadas
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                             | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    //ponteiro para a fun��o debugCallback (implementada nessa mesma classe)
    createInfo.pfnUserCallback = debug_callback;
    createInfo.pUserData = nullptr; // Optional

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
         if (func(m_instance, &createInfo, nullptr, &m_debugCallback) != VK_SUCCESS){
             throw std::runtime_error("failed to set up debug callback!");
         }
    }
    else {
        LOGE << "Debug extension not present!";
        throw std::runtime_error("Debug extension not present!");
    }
}

VkBool32 VulkanContext::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << "\n\n";
    LOGV << pCallbackData->pMessage;
    return VK_FALSE;
}


_EAGLE_END
