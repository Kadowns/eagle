//
// Created by Novak on 08/05/2019.
//

#include <set>

#include "VulkanContext.h"
#include "VulkanHelper.h"
#include <eagle/renderer/vulkan/VulkanCommandBuffer.h>
#include "eagle/Window.h"


#include "eagle/Log.h"
#include <eagle/renderer/vulkan/VulkanConverter.h>

EG_BEGIN

bool VulkanContext::enableValidationLayers = false;

VulkanContext::VulkanContext() {

}

VulkanContext::~VulkanContext() = default;



void VulkanContext::destroy() {

    EG_CORE_TRACE("Terminating vulkan!");

    VK_CALL
    vkDeviceWaitIdle(m_device);

    m_listener.detach();

    m_vertexBuffers.clear();
    m_indexBuffers.clear();

    cleanup_swapchain();
    m_descriptorSets.clear();
    m_descriptorSetsLayouts.clear();
    m_uniformBuffers.clear();
    m_textures.clear();
    m_shaders.clear();
    m_computeShaders.clear();
    m_present.renderPass.reset();
    m_renderPasses.clear();
    m_framebuffers.clear();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_CALL
        vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
        VK_CALL
        vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
        VK_CALL
        vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
    }


    VK_CALL
    vkDestroyCommandPool(m_device, m_graphicsCommandPool, nullptr);

    VK_CALL
    vkDestroyCommandPool(m_device, m_computeCommandPool, nullptr);

    VK_CALL
    vkDestroyDevice(m_device, nullptr);

    VK_CALL
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (enableValidationLayers) {
        VK_CALL
        DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    VK_CALL
    vkDestroyInstance(m_instance, nullptr);

    EG_CORE_TRACE("Vulkan terminated!");
}


void VulkanContext::create_instance() {

    EG_CORE_TRACE("Creating vulkan instance!");

    if (enableValidationLayers && !validation_layers_supported()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "NAME_ME";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "eagle";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = std::move(get_extensions());
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debug_callback;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VK_CALL_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_instance)) {
        throw std::runtime_error("failed to create instance!");
    }

    EG_CORE_TRACE("Vulkan instance created!");
}

void VulkanContext::create_debug_callback() {

    EG_CORE_TRACE("Setting up debug callback!");

    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debug_callback;
    createInfo.pUserData = &VkDebugInfo::get_last_call();

    VK_CALL_ASSERT(CreateDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debugMessenger)) {
        throw std::runtime_error("failed to setup debug callback!");
    }
    EG_CORE_TRACE("Debug callback ready!");
}

void VulkanContext::bind_physical_device() {

    EG_CORE_TRACE("Selecting physical device!");

    //verifica quantas placas de video suportam a vulkan no pc
    uint32_t deviceCount = 0;
    VK_CALL
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    EG_CORE_INFO_F("Number of supporting devices: {0}", deviceCount);

    //se n�o houver nenhuma placa de video que suporta, para o programa
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    //lista todas as placas de video disponiveis
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CALL
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

#ifndef VK_USE_PLATFORM_ANDROID_KHR
    //map usado pra rankear a melhor placa de video
    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto &device : devices) {
        //verifica a qualidade da placa de video
        int score = evaluate_device(device);

        //guarda a referencia da placa de video junto com seu score
        candidates.insert(std::make_pair(score, device));
    }

    //caso o score da melhor placa de video seja maior que zero, iremos utilizar ela
    if (candidates.rbegin()->first > 0) {
        m_physicalDevice = candidates.rbegin()->second;
    } else {
        //caso nenhuma preste
        throw std::runtime_error("no suitable GPU found!");
    }
#endif
    m_physicalDevice = devices[0];


    EG_CORE_TRACE("Physical device selected!");
}

int VulkanContext::evaluate_device(VkPhysicalDevice device) {

    EG_CORE_TRACE("Evaluating device!");
    VkPhysicalDeviceProperties physicalDeviceProperties = {};

    VK_CALL
    vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
    //features
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VK_CALL
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if (!deviceFeatures.samplerAnisotropy) {
        return 0;
    }

//    if (!deviceFeatures.geometryShader) {
//        return 0;
//    }

    if (!deviceFeatures.samplerAnisotropy) {
        return 0;
    }

    if (!deviceFeatures.textureCompressionASTC_LDR && !deviceFeatures.textureCompressionETC2 &&
        !deviceFeatures.textureCompressionBC) {
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


    //verifica suporte de swapchain
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


    EG_CORE_TRACE_F("Device suitable! score:{0}", score);

    return score;
}


bool VulkanContext::validation_layers_supported() {
    EG_CORE_TRACE("Checking validation layer support!");
    uint32_t layerCount;
    VK_CALL
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    VK_CALL
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers) {
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

VkBool32 VulkanContext::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    VkDebugInfo::VkCall info = VkDebugInfo::m_callInfo;//*((VkDebugInfo::VkCall *) pUserData);

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::TRACE, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::INFO, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::WARN, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::ERR, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::CRITICAL,
                          pCallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

VulkanContext::QueueFamilyIndices VulkanContext::find_family_indices(VkPhysicalDevice device) {
    EG_CORE_TRACE("Finding queue family indices!");
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    VK_CALL
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    VK_CALL
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily : queueFamilies) {

        VkBool32 presentSupport = false;
        VK_CALL
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
        }
        if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            indices.computeFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
        i++;
    }

    if (!indices.computeFamily.has_value()){
        EG_WARNING("No dedicated compute queue found! Searching for any suitable queue family");
        i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT){
                indices.computeFamily = i;
                break;
            }
            i++;
        }
    }

    return indices;
}

bool VulkanContext::check_device_extension_support(VkPhysicalDevice device) {

    EG_CORE_TRACE("Checking device extensions!");

    uint32_t extensionCount;
    VK_CALL
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    //lista as extens�es disponiveis
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    VK_CALL
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    //requiredExtensions possuiu todas as extens�es necess�rias
    //nesse loop n�s pegamos as extens�es que possuimos e retiramos elas do requiredExtensions
    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    //caso todas as extens�es tenham sido apagadas do required extensions
    //significa que possuimos todas as que s�o necess�rias
    return requiredExtensions.empty();
}

VulkanContext::SwapChainSupportDetails VulkanContext::query_swapchain_support(VkPhysicalDevice device) {

    EG_CORE_TRACE("Querying swapchain support!");
    SwapChainSupportDetails details;

    VK_CALL
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    //Formats---------------------------------------
    uint32_t formatCount;
    VK_CALL
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        VK_CALL
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }
    //----------------------------------------------


    //Presentation Modes----------------------------
    uint32_t presentModeCount;
    VK_CALL
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        VK_CALL
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }
    //----------------------------------------------
    return details;
}

void VulkanContext::create_logical_device() {
    EG_CORE_TRACE("Creating logical device!");
    QueueFamilyIndices indices = find_family_indices(m_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value(), indices.computeFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VK_CALL_ASSERT(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device)) {
        throw std::runtime_error("failed to create logical device!");
    }

    VK_CALL vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    VK_CALL vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    VK_CALL vkGetDeviceQueue(m_device, indices.computeFamily.value(), 0, &m_computeQueue);

    EG_CORE_TRACE("Logical device created!");
}

VkSurfaceFormatKHR VulkanContext::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) {

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        EG_CORE_INFO("Swapchain surface format: VK_FORMAT_UNDEFINED");
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            EG_CORE_INFO("Swapchain surface format: VK_FORMAT_B8G8R8A8_UNORM");
            return availableFormat;
        }
    }

    EG_CORE_INFO_F("Swapchain surface format: {0}", formats[0].format);
    return formats[0];
}

void VulkanContext::create_swapchain() {

    EG_CORE_TRACE("Creating swapchain!");
    //Verifica o suporte de swapChains
    SwapChainSupportDetails swapChainSupport = query_swapchain_support(m_physicalDevice);

    //Define que tipo de imagem vai ser gerada
    VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);

    //Define qual vai ser a maneira que a swap chain vai se comportar (queue, ou simplesmente jogando tudo na tela)
    VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.presentModes);

    //verifica se a swap chain consegue gerar imagens de qualque tamanho e, caso n�o consiga, define qual vai ser o tamanho permitido
    VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities);

    m_present.swapchainFormat = surfaceFormat.format;
    m_present.extent2D = extent;

    //caso maxImageCount seja 0, a queue da swapchain suporta qualquer quantidade de imagens
    m_present.imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && m_present.imageCount > swapChainSupport.capabilities.maxImageCount) {
        m_present.imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    //Informa��es da swapchain
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = m_present.imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//VK_IMAGE_USAGE_TRANSFER_DST_BIT para post process
    //-----------------------------

    QueueFamilyIndices indices = find_family_indices(m_physicalDevice);
    std::set<uint32_t> uniqueFamilies = {(uint32_t) indices.graphicsFamily.value(),
                                         (uint32_t) indices.presentFamily.value(),
                                         (uint32_t) indices.computeFamily.value()};


    std::vector<uint32_t> queueFamilyIndices(uniqueFamilies.begin(), uniqueFamilies.end());

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CALL_ASSERT(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_present.swapchain)) {
        throw std::runtime_error("failed to create swap chain!");
    }
}

VkPresentModeKHR VulkanContext::choose_swap_present_mode(const std::vector<VkPresentModeKHR> &presentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto &availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_MAILBOX_KHR");
            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_IMMEDIATE_KHR");
            bestMode = availablePresentMode;
        }
    }
    EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_FIFO_KHR");
    return bestMode;
}

VkExtent2D VulkanContext::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        EG_CORE_INFO_F("Swap extent: width:{0} height:{1}", capabilities.currentExtent.width,
                       capabilities.currentExtent.height);
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {};
        actualExtent.width = std::clamp<uint32_t>(m_window->width(), capabilities.minImageExtent.width,
                                                  capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp<uint32_t>(m_window->height(), capabilities.minImageExtent.height,
                                                   capabilities.maxImageExtent.height);

        EG_CORE_INFO_F("Swap extent: width:{0} height:{1}", actualExtent.width, actualExtent.height);
        return actualExtent;
    }
}

void VulkanContext::create_render_pass() {

    EG_CORE_TRACE("Creating a present render pass!");

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_present.swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VulkanHelper::find_depth_format(m_physicalDevice);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VulkanRenderPassCreateInfo createInfo = {};
    createInfo.device = m_device;

    m_present.renderPass = std::make_shared<VulkanRenderPass>(createInfo, colorAttachment, depthAttachment);

    EG_CORE_TRACE("Present render pass created!");
}

void VulkanContext::create_framebuffers() {
    EG_CORE_TRACE("Creating present framebuffers!");

    VK_CALL vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &m_present.imageCount, nullptr);
    EG_CORE_INFO_F("Number of swapchain images: {0}", m_present.imageCount);

    std::vector<VkImage> swapchainImages(m_present.imageCount);

    VK_CALL vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &m_present.imageCount, swapchainImages.data());

    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.width = m_present.extent2D.width;
    imageCreateInfo.height = m_present.extent2D.height;
    imageCreateInfo.format = VulkanConverter::to_eg(m_present.swapchainFormat);
    imageCreateInfo.aspects = {ImageAspect::COLOR};
    imageCreateInfo.usages = {ImageUsage::COLOR_ATTACHMENT};

    VulkanImageCreateInfo nativeImageCreateInfo = {};
    nativeImageCreateInfo.physicalDevice = m_physicalDevice;
    nativeImageCreateInfo.device = m_device;
    nativeImageCreateInfo.graphicsQueue = m_graphicsQueue;
    nativeImageCreateInfo.commandPool = m_graphicsCommandPool;
    nativeImageCreateInfo.imageCount = m_present.imageCount;

    Reference<VulkanImage> colorImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo, swapchainImages);

    //depth
    imageCreateInfo.format = VulkanConverter::to_eg(VulkanHelper::find_depth_format(m_physicalDevice));
    imageCreateInfo.aspects = {ImageAspect::DEPTH};
    imageCreateInfo.usages = {ImageUsage::DEPTH_STENCIL_ATTACHMENT};
    imageCreateInfo.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    imageCreateInfo.tiling = ImageTiling::OPTIMAL;
    imageCreateInfo.memoryProperties = {MemoryProperty::DEVICE_LOCAL};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    if (imageCreateInfo.format == Format::D32_SFLOAT_S8_UINT || imageCreateInfo.format == Format::D24_UNORM_S8_UINT) {
        imageCreateInfo.aspects.emplace_back(ImageAspect::STENCIL);
        EG_CORE_INFO("Depth format allows stencil aspect");
    }

    Reference<VulkanImage> depthImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo);

    FramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.width = m_present.extent2D.width;
    framebufferCreateInfo.height = m_present.extent2D.height;
    framebufferCreateInfo.renderPass = m_present.renderPass;
    framebufferCreateInfo.attachments = {colorImage, depthImage};

    VulkanFramebufferCreateInfo nativeFramebufferCreateInfo = {};
    nativeFramebufferCreateInfo.device = m_device;
    nativeFramebufferCreateInfo.imageCount = m_present.imageCount;

    m_present.framebuffer = std::make_shared<VulkanFramebuffer>(framebufferCreateInfo, nativeFramebufferCreateInfo);

    EG_CORE_TRACE("Framebuffers created!");
}


void VulkanContext::create_command_pool() {

    EG_CORE_TRACE("Creating command pool!");

    QueueFamilyIndices queueFamilyIndices = find_family_indices(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL_ASSERT(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_graphicsCommandPool)) {
        throw std::runtime_error("failed to create graphics command pool!");
    }

    poolInfo.queueFamilyIndex = queueFamilyIndices.computeFamily.value();

    VK_CALL_ASSERT(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_computeCommandPool)){
        throw std::runtime_error("failed to create compute command pool!");
    }

    EG_CORE_TRACE("Command pool created!");
}

void VulkanContext::allocate_command_buffers() {

    EG_CORE_TRACE("Allocating graphics command buffers!");

    m_commandBuffers.resize(m_present.imageCount);
    for (int i = 0; i < m_commandBuffers.size(); i++) {
        m_commandBuffers[i] = std::make_shared<VulkanCommandBuffer>(
                m_device,
                m_graphicsCommandPool,
                m_present.imageIndex
        );
    }

    EG_CORE_TRACE("Graphics command buffers allocated!");
}

void VulkanContext::create_sync_objects() {

    EG_CORE_TRACE("Creating sync objects!");

    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_CALL
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create sync objects for a frame!");
        }
    }

    EG_CORE_TRACE("Sync objects created!");
}


void VulkanContext::recreate_swapchain() {
    EG_CORE_TRACE("Recreating swapchain!");

    VK_CALL
    vkDeviceWaitIdle(m_device);

    //Waits until window is visible
    while (m_window->is_minimized()) {
        m_window->wait_native_events();
    }

    cleanup_swapchain();

    create_swapchain();
    allocate_command_buffers();
    create_framebuffers();
    recreate_objects();

    m_eventBus->emit(OnRenderingContextRecreated{this});

    EG_CORE_TRACE("Swapchain recreated!");
}

void VulkanContext::recreate_objects() {
    for (auto &shader : m_shaders) {
        shader->create_pipeline();
    }

    for (auto &uniformBuffer : m_uniformBuffers) {
        uniformBuffer->recreate(m_present.imageCount);
    }

    for (auto& storageBuffer : m_storageBuffers){
        storageBuffer->recreate(m_present.imageCount);
    }

    for (auto &descriptorSet : m_descriptorSets) {
        descriptorSet->recreate(m_present.imageCount);
    }

    for (auto& computeShader : m_computeShaders){
        computeShader->recreate(m_present.imageCount);
    }
}

void VulkanContext::clear_objects() {
    VulkanCleaner::clear();

    for (auto& computeShader : m_computeShaders){
        computeShader->clear_descriptor_set();
    }

    for (auto &descriptorSet : m_descriptorSets) {
        descriptorSet->cleanup();
    }

    for (auto &uniformBuffer : m_uniformBuffers) {
        uniformBuffer->cleanup();
    }

    for (auto& storageBuffer : m_storageBuffers){
        storageBuffer->cleanup();
    }

    for (auto &shader : m_shaders) {
        shader->cleanup_pipeline();
    }
}


void VulkanContext::cleanup_swapchain() {
    EG_CORE_TRACE("Clearing swapchain!");

    clear_objects();

    m_commandBuffers.clear();

    m_present.framebuffer.reset();

    VK_CALL vkDestroySwapchainKHR(m_device, m_present.swapchain, nullptr);
    EG_CORE_TRACE("Swapchain cleared!");
}

Handle <Shader>
VulkanContext::create_shader(const ShaderCreateInfo &createInfo) {
    EG_CORE_TRACE("Creating a vulkan shader!");

    VulkanShaderCreateInfo nativeCreateInfo = {};
    nativeCreateInfo.device = m_device;
    nativeCreateInfo.pExtent = &m_present.extent2D;
    m_shaders.emplace_back(std::make_shared<VulkanShader>(createInfo, nativeCreateInfo));
    return m_shaders.back();
}

Handle<ComputeShader>
VulkanContext::create_compute_shader(const std::string &path) {
    VulkanComputeShaderCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.commandPool = m_computeCommandPool;
    createInfo.imageIndex = &m_present.imageIndex;
    createInfo.bufferCount = m_present.imageCount;
    createInfo.computeQueue = m_computeQueue;
    m_computeShaders.emplace_back(std::make_shared<VulkanComputeShader>(path, createInfo));
    return m_computeShaders.back();
}

Handle<VertexBuffer>
VulkanContext::create_vertex_buffer(void *vertices, uint32_t size, const VertexLayout &vertexLayout,
                                    UpdateType usage) {
    EG_CORE_TRACE("Creating a vulkan vertex buffer!");
    VulkanVertexBufferCreateInfo createInfo(vertexLayout);
    createInfo.data = vertices;
    createInfo.size = size;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_graphicsCommandPool;
    createInfo.graphicsQueue = m_graphicsQueue;
    createInfo.bufferCount = usage == UpdateType::HOST ? m_present.imageCount : 1;
    m_vertexBuffers.emplace_back(std::make_shared<VulkanVertexBuffer>(m_device, createInfo, usage));
    return m_vertexBuffers.back();
}

Handle<IndexBuffer>
VulkanContext::create_index_buffer(void *indexData, size_t indexCount, IndexBufferType indexType,
                                   UpdateType usage) {
    EG_CORE_TRACE("Creating a vulkan index buffer!");
    VulkanIndexBufferCreateInfo createInfo = {};
    createInfo.graphicsQueue = m_graphicsQueue;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_graphicsCommandPool;
    createInfo.bufferCount = usage == UpdateType::HOST ? m_present.imageCount : 1;
    m_indexBuffers.emplace_back(std::make_shared<VulkanIndexBuffer>(m_device, createInfo, indexData, indexCount, indexType, usage));
    return m_indexBuffers.back();
}

Handle<UniformBuffer>
VulkanContext::create_uniform_buffer(size_t size, void *data) {
    EG_CORE_TRACE("Creating a vulkan uniform buffer!");
    VulkanUniformBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_present.imageCount;
    m_uniformBuffers.emplace_back(std::make_shared<VulkanUniformBuffer>(createInfo, size, data));
    return m_uniformBuffers.back();
}

Handle<StorageBuffer>
VulkanContext::create_storage_buffer(size_t size, void *data, UpdateType usage) {
    EG_CORE_TRACE("Creating a vulkan storage buffer!");
    VulkanStorageBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_present.imageCount;
    createInfo.commandPool = m_graphicsCommandPool;
    createInfo.graphicsQueue = m_graphicsQueue;
    m_storageBuffers.emplace_back(std::make_shared<VulkanStorageBuffer>(createInfo, size, data, usage));
    return m_storageBuffers.back();
}


Handle<DescriptorSetLayout>
VulkanContext::create_descriptor_set_layout(const std::vector<DescriptorBindingDescription> &bindings) {
    m_descriptorSetsLayouts.emplace_back(std::make_shared<VulkanDescriptorSetLayout>(m_device, bindings));
    return m_descriptorSetsLayouts.back();
}


Handle<DescriptorSet>
VulkanContext::create_descriptor_set(const Reference<DescriptorSetLayout>& descriptorLayout,
                                     const std::vector<Reference<DescriptorItem>> &descriptorItems) {
    EG_CORE_TRACE("Creating a vulkan descriptor set!");
    VulkanDescriptorSetCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.bufferCount = m_present.imageCount;

    m_descriptorSets.emplace_back(std::make_shared<VulkanDescriptorSet>(
            std::static_pointer_cast<VulkanDescriptorSetLayout>(descriptorLayout),
            descriptorItems,
            createInfo
            )
    );
    return m_descriptorSets.back();
}

Handle<Texture>
VulkanContext::create_texture(const TextureCreateInfo &createInfo) {

    EG_CORE_TRACE("Creating a vulkan texture!");
    VulkanTextureCreateInfo vulkanTextureCreateInfo = {};
    vulkanTextureCreateInfo.device = m_device;
    vulkanTextureCreateInfo.physicalDevice = m_physicalDevice;
    vulkanTextureCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanTextureCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanTextureCreateInfo.imageCount = m_present.imageCount;

    m_textures.emplace_back(std::make_shared<VulkanTexture>(createInfo, vulkanTextureCreateInfo));
    return m_textures.back();
}

Handle<Image>
VulkanContext::create_image(const ImageCreateInfo &createInfo) {
    EG_CORE_TRACE("Creating a vulkan image!");
    VulkanImageCreateInfo vulkanImageCreateInfo = {};
    vulkanImageCreateInfo.device = m_device;
    vulkanImageCreateInfo.physicalDevice = m_physicalDevice;
    vulkanImageCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanImageCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanImageCreateInfo.imageCount = m_present.imageCount;

    m_images.emplace_back(std::make_shared<VulkanImage>(createInfo, vulkanImageCreateInfo));
    return m_images.back();
}


Handle<RenderPass> VulkanContext::create_render_pass(const std::vector<RenderAttachmentDescription> &colorAttachments,
                                                     const RenderAttachmentDescription &depthAttachment) {
    VulkanRenderPassCreateInfo createInfo = {};
    createInfo.device = m_device;

    m_renderPasses.emplace_back(std::make_shared<VulkanRenderPass>(createInfo, colorAttachments, depthAttachment));
    return m_renderPasses.back();
}

Handle<Framebuffer> VulkanContext::create_framebuffer(const FramebufferCreateInfo &createInfo) {

    VulkanFramebufferCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;
    vulkanCreateInfo.imageCount = m_present.imageCount;

    m_framebuffers.emplace_back(std::make_shared<VulkanFramebuffer>(createInfo, vulkanCreateInfo));
    return m_framebuffers.back();
}


void VulkanContext::create_offscreen_render_pass() {
    // + 1 for depth attachment
//    std::array<VkAttachmentDescription, 2> attachmentDescriptions = {};
//    // Color attachment
//
//    attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
//    attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//    attachmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM;
//    attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//    attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//    attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
//    attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//    attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//    attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_GENERAL;
//    attachmentDescriptions[1].format = find_depth_format();
//    attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//    attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//
//    VkAttachmentReference colorReference = {};
//    colorReference.attachment = 0;
//    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//    VkAttachmentReference depthReference = {};
//    depthReference.attachment = 1;
//    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//    VkSubpassDescription subpassDescription = {};
//    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//    subpassDescription.colorAttachmentCount = 1;
//    subpassDescription.pColorAttachments = &colorReference;
//    subpassDescription.pDepthStencilAttachment = &depthReference;
//
//    // Use subpass dependencies for layout transitions
//    std::vector<VkSubpassDependency> dependencies(attachmentDescriptions.size());
//    for (size_t i = 0; i < dependencies.size(); i++){
//
//        dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//        if (i != dependencies.size() - 1){
//            //color
//            dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
//            dependencies[i].dstSubpass = 0;
//            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//            dependencies[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//            dependencies[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//        } else {
//            //depth
//            dependencies[i].srcSubpass = 0;
//            dependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
//            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//            dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//            dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//        }
//    }
//
//
//    VkRenderPassCreateInfo renderPassInfo = {};
//    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
//    renderPassInfo.pAttachments = attachmentDescriptions.data();
//    renderPassInfo.subpassCount = 1;
//    renderPassInfo.pSubpasses = &subpassDescription;
//    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
//    renderPassInfo.pDependencies = dependencies.data();
//
//    VK_CALL vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_present.offscreenPass);
}

Reference<CommandBuffer> VulkanContext::main_command_buffer() {
    return m_commandBuffers[m_present.imageIndex];
}

bool VulkanContext::prepare_frame() {
    VK_CALL vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result;
    VK_CALL
    result = vkAcquireNextImageKHR(m_device, m_present.swapchain, std::numeric_limits<uint64_t>::max(),
                                   m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_present.imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swapchain image!");
    }

    //updates dirty buffers-------------------------
    VulkanCleaner::flush(m_present.imageIndex);
    return true;
}

void VulkanContext::submit_command_buffer(const Reference<CommandBuffer> &commandBuffer) {

    auto vcb = std::static_pointer_cast<VulkanCommandBuffer>(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vcb->native_command_buffer();

    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CALL vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

    VkResult result = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);

    if (result != VK_SUCCESS){
        throw std::runtime_error("failed to submit command buffer!");
    }
}

void VulkanContext::present_frame() {

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore waitSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapChains[] = {m_present.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_present.imageIndex;

    VK_CALL
    VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);


    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowResized) {
        m_windowResized = false;
        recreate_swapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swapchain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanContext::destroy_texture_2d(const Reference<Texture> &texture) {
    m_textures.erase(std::find(m_textures.begin(), m_textures.end(), std::static_pointer_cast<VulkanTexture>(texture)));
}

Reference<RenderPass> VulkanContext::main_render_pass() {
    return m_present.renderPass;
}

Reference<Framebuffer> VulkanContext::main_frambuffer() {
    return m_present.framebuffer;
}

std::vector<const char*> VulkanContext::get_extensions() {

    auto extensions = std::move(get_platform_extensions());
    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pCallback) {

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//Proxys pro debug-----------------------------------
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}


EG_END
