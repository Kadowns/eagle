//
// Created by Novak on 08/05/2019.
//



#include "eagle/renderer/vulkan/VulkanContext.h"
#include "eagle/core/Core.h"
#include "eagle/core/Log.h"

#include <set>

#include <GLFW/glfw3.h>


_EAGLE_BEGIN

bool VulkanContext::enableValidationLayers = true;

VulkanContext::VulkanContext(uint32_t width, uint32_t height) :
    RenderingContext(width, height){
    EAGLE_SET_INFO(EAGLE_RENDERING_CONTEXT, "Vulkan");
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::init() {
    EG_TRACE("Initializing vulkan context!");
    create_instance();
    create_debug_callback();
    create_surface();
    bind_physical_device();
    create_logical_device();

}

void VulkanContext::deinit() {

    VK_CALL vkDestroyDevice(m_device, nullptr);

    VK_CALL vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (enableValidationLayers) {
        VK_CALL DestroyDebugUtilsMessengerEXT(m_instance, m_debugCallback, nullptr);
    }


    VK_CALL vkDestroyInstance(m_instance, nullptr);

}

void VulkanContext::refresh() {

}

void VulkanContext::create_instance() {

    EG_TRACE("Creating vulkan instance!");

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


    //define quais validations layers vão estar ativas
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    ///---------------------------------------------------------------

    VK_CALL_IF(vkCreateInstance(&createInfo, nullptr, &m_instance)){
        throw std::runtime_error("failed to create vkInstance!");
    }

    EG_TRACE("Vulkan instance created!");
}

void VulkanContext::create_debug_callback() {

    EG_TRACE("Setting up debug callback!");

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

    //ponteiro pro callback
    createInfo.pfnUserCallback = debug_callback;

    //struct que guarda as informações da chamada (nome do arquivo, função e linha da chamada)
    createInfo.pUserData = &m_callInfo;


    VK_CALL_IF(CreateDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugCallback)){
        throw std::runtime_error("failed to setup debug callback!");
    }
    EG_TRACE("Debug callback ready!");
}

void VulkanContext::create_surface() {

    EG_TRACE("Creating window surface!");
    VK_CALL_IF(glfwCreateWindowSurface( m_instance, m_windowHandle, nullptr, &m_surface)){
        throw std::runtime_error("failed to create window surface!");
    }
    EG_TRACE("Window surface created!");
}

void VulkanContext::bind_physical_device() {

    EG_TRACE("Selecting physical device!");

    //verifica quantas placas de video suportam a vulkan no pc
    uint32_t deviceCount = 0;
    VK_CALL vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    EG_INFO_F("Number of supporting devices: {0}", deviceCount);

    //se n�o houver nenhuma placa de video que suporta, para o programa
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    //lista todas as placas de video disponiveis
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CALL vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    //map usado pra rankear a melhor placa de video
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        //verifica a qualidade da placa de video
        int score = evaluate_device(device);

        //guarda a referencia da placa de video junto com seu score
        candidates.insert(std::make_pair(score, device));
    }

    //caso o score da melhor placa de video seja maior que zero, iremos utilizar ela
    if (candidates.rbegin()->first > 0) {
        m_physicalDevice = candidates.rbegin()->second;
    }
    else {
        //caso nenhuma preste
        throw std::runtime_error("no suitable GPU found!");
    }

    EG_TRACE("Physical device selected!");
}

int VulkanContext::evaluate_device(VkPhysicalDevice device) {

    EG_TRACE("Evaluating device!");
    VkPhysicalDeviceProperties physicalDeviceProperties = {};

    VK_CALL vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
    //features
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VK_CALL vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    //se n�o tiver geometry shader, pula fora
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

    if (!deviceFeatures.samplerAnisotropy) {
        return 0;
    }

    if (!deviceFeatures.textureCompressionASTC_LDR && !deviceFeatures.textureCompressionETC2 && !deviceFeatures.textureCompressionBC) {
        return 0;
    }

    //Verifica se possui suporte para os queue families
    auto families = find_family_indices(device);
    if (!families.isComplete())
        return 0;

    //verifica se possui as extens�es necess�rias
    bool extensionsSupported = check_device_extension_support(device);
    if (!extensionsSupported)
        return 0;


    //Verifica se possuiu suporte de SwapChains (� basicamente uma queue das imagens que foram renderizadas e v�o ser printadas na tela)
    SwapChainSupportDetails swapChainSupport = query_swapchain_support(device);
    if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
        return 0;
    }

    //TODO------------fazer um sistema de scores mais eficiente
    int score = 0;

    if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 999;
    }

    score += physicalDeviceProperties.limits.maxImageDimension2D;

    EG_TRACE_F("Device suitable! score:{0}", score);

    return score;
}

void VulkanContext::set_window_handle(void *window) {

    m_windowHandle = (GLFWwindow*)window;
    EG_TRACE("Window handle set!");
}

bool VulkanContext::validation_layers_supported(){
    EG_TRACE("Checking validation layer support!");
    uint32_t layerCount;
    VK_CALL vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    VK_CALL vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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
    EG_TRACE("Getting required extensions!");
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}



VkBool32 VulkanContext::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    VkCallInfo info = *((VkCallInfo*)pUserData);

    switch(messageSeverity){
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, spdlog::level::trace, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, spdlog::level::info, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, spdlog::level::warn, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, spdlog::level::err, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, spdlog::level::critical, pCallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

VulkanContext::QueueFamilyIndices VulkanContext::find_family_indices(VkPhysicalDevice device) {
    EG_TRACE("Finding queue family indices!");
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    VK_CALL vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    VK_CALL vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {

        VkBool32 presentSupport = false;
        VK_CALL vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

bool VulkanContext::check_device_extension_support(VkPhysicalDevice device) {

    EG_TRACE("Checking device extensions!");

    uint32_t extensionCount;
    VK_CALL vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    //lista as extens�es disponiveis
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VK_CALL vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    //requiredExtensions possuiu todas as extens�es necess�rias
    //nesse loop n�s pegamos as extens�es que possuimos e retiramos elas do requiredExtensions
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    //caso todas as extens�es tenham sido apagadas do required extensions
    //significa que possuimos todas as que s�o necess�rias
    return requiredExtensions.empty();
}

VulkanContext::SwapChainSupportDetails VulkanContext::query_swapchain_support(VkPhysicalDevice device) {

    EG_TRACE("Querying swapchain support!");
    SwapChainSupportDetails details;

    VK_CALL vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    //Formats---------------------------------------
    uint32_t formatCount;
    VK_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        VK_CALL vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }
    //----------------------------------------------


    //Presentation Modes----------------------------
    uint32_t presentModeCount;
    VK_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        VK_CALL vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }
    //----------------------------------------------
    return details;
}

void VulkanContext::create_logical_device() {
    EG_TRACE("Creating logical device!");
    QueueFamilyIndices indices = find_family_indices(m_physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VK_CALL_IF(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device)){
        throw std::runtime_error("failed to create logical device!");
    }

    VK_CALL vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);

    EG_TRACE("Logical device created!");
}


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pCallback) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//Proxys pro debug-----------------------------------
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}


_EAGLE_END
