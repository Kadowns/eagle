//
// Created by Novak on 08/05/2019.
//

#include <eagle/renderer/vulkan/vulkan_context.h>
#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/renderer/vulkan/vulkan_command_buffer.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_semaphore.h>

#include <eagle/log.h>
#include <eagle/window.h>

namespace eagle {

#ifdef EG_PLATFORM_WIN32
    bool VulkanContext::enableValidationLayers = true;
#else
    bool VulkanContext::enableValidationLayers = false;
#endif


VulkanContext::VulkanContext() {
    EG_LOG_CREATE("vulkan");
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::destroy() {

    EG_TRACE("eagle", "Terminating vulkan!");

    VK_CALL vkDeviceWaitIdle(m_device);

    cleanup_swapchain();

    m_present.renderPass.reset();

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

    EG_TRACE("eagle","Vulkan terminated!");
}


void VulkanContext::create_instance() {

    EG_TRACE("eagle","Creating vulkan instance!");

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

    EG_TRACE("eagle","Vulkan instance created!");
}

void VulkanContext::create_debug_callback() {

    EG_TRACE("eagle","Setting up debug callback!");

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
    EG_TRACE("eagle","Debug callback ready!");
}

void VulkanContext::bind_physical_device() {

    EG_TRACE("eagle","Selecting physical device!");

    //verifica quantas placas de video suportam a vulkan no pc
    uint32_t deviceCount = 0;
    VK_CALL
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    EG_INFO("eagle","Number of supporting devices: {0}", deviceCount);

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


    EG_TRACE("eagle","Physical device selected!");
}

int VulkanContext::evaluate_device(VkPhysicalDevice device) {

    EG_TRACE("eagle","Evaluating device!");
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


    EG_TRACE("eagle","Device suitable! score:{0}", score);

    return score;
}


bool VulkanContext::validation_layers_supported() {
    EG_TRACE("eagle","Checking validation layer support!");
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

    VkDebugInfo::VkCall& info = VkDebugInfo::m_callInfo;//*((VkDebugInfo::VkCall *) pUserData);

#define VK_LOG(level, message) spdlog::get("eagle")->log(spdlog::source_loc{info.fileName, info.line, info.funcName}, level, message);

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            VK_LOG(spdlog::level::trace, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            VK_LOG(spdlog::level::info, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            VK_LOG(spdlog::level::warn, pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            VK_LOG(spdlog::level::err, pCallbackData->pMessage);
            break;
        default:
            VK_LOG(spdlog::level::critical, pCallbackData->pMessage);
            break;
    }

#undef VK_LOG
    return VK_FALSE;
}

VulkanContext::QueueFamilyIndices VulkanContext::find_family_indices(VkPhysicalDevice device) {
    EG_TRACE("eagle","Finding queue family indices!");
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
        EG_WARNING("eagle", "No dedicated compute queue found! Searching for any suitable queue family");
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

    EG_TRACE("eagle","Checking device extensions!");

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

    EG_TRACE("eagle","Querying swapchain support!");
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
    EG_TRACE("eagle","Creating logical device!");
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

    VulkanDeleterInfo deleterCreateInfo = {};
    deleterCreateInfo.device = m_device;

    m_deleter = std::make_shared<VulkanDeleter>(deleterCreateInfo);

    EG_TRACE("eagle","Logical device created!");
}

VkSurfaceFormatKHR VulkanContext::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) {

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        EG_INFO("eagle","Swapchain surface format: VK_FORMAT_UNDEFINED");
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            EG_INFO("eagle","Swapchain surface format: VK_FORMAT_B8G8R8A8_UNORM");
            return availableFormat;
        }
    }

    EG_INFO("eagle","Swapchain surface format: {0}", formats[0].format);
    return formats[0];
}

void VulkanContext::create_swapchain() {

    EG_TRACE("eagle","Creating swapchain!");
    //Verifica o suporte de swapChains
    SwapChainSupportDetails swapChainSupport = query_swapchain_support(m_physicalDevice);

    //Define que tipo de imagem vai ser gerada
    VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats);

    //Define qual vai ser a maneira que a swap chain vai se comportar (queue, ou simplesmente jogando tudo na tela)
    VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.presentModes);

    //verifica se a swap chain consegue gerar imagens de qualque tamanho e, caso n�o consiga, define qual vai ser o tamanho permitido
    VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities);

    m_properties.presentFormat = VulkanConverter::to_eg(surfaceFormat.format);
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
            EG_INFO("eagle","Swapchain present mode: VK_PRESENT_MODE_MAILBOX_KHR");
            return availablePresentMode;
        } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            EG_INFO("eagle","Swapchain present mode: VK_PRESENT_MODE_IMMEDIATE_KHR");
            bestMode = availablePresentMode;
        }
    }
    EG_INFO("eagle","Swapchain present mode: VK_PRESENT_MODE_FIFO_KHR");
    return bestMode;
}

VkExtent2D VulkanContext::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        EG_INFO("eagle","Swap extent: width:{0} height:{1}", capabilities.currentExtent.width,
                       capabilities.currentExtent.height);
        return capabilities.currentExtent;
    } else {
        VkExtent2D actualExtent = {};
        actualExtent.width = std::clamp<uint32_t>(m_window->width(), capabilities.minImageExtent.width,
                                                  capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp<uint32_t>(m_window->height(), capabilities.minImageExtent.height,
                                                   capabilities.maxImageExtent.height);

        EG_INFO("eagle","Swap extent: width:{0} height:{1}", actualExtent.width, actualExtent.height);
        return actualExtent;
    }
}

void VulkanContext::create_render_pass() {

    EG_TRACE("eagle","Creating a present render pass!");

    RenderAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_properties.presentFormat;
    colorAttachment.loadOp = AttachmentLoadOperator::CLEAR;
    colorAttachment.storeOp = AttachmentStoreOperator::STORE;
    colorAttachment.stencilLoadOp = AttachmentLoadOperator::DONT_CARE;
    colorAttachment.stencilStoreOp = AttachmentStoreOperator::DONT_CARE;
    colorAttachment.initialLayout = ImageLayout::UNDEFINED;
    colorAttachment.finalLayout = ImageLayout::PRESENT_SRC_KHR;

    m_properties.depthFormat = VulkanConverter::to_eg(VulkanHelper::find_depth_format(m_physicalDevice));

    RenderAttachmentDescription depthAttachment = {};
    depthAttachment.format = m_properties.depthFormat;
    depthAttachment.loadOp = AttachmentLoadOperator::CLEAR;
    depthAttachment.storeOp = AttachmentStoreOperator::DONT_CARE;
    depthAttachment.stencilLoadOp = AttachmentLoadOperator::DONT_CARE;
    depthAttachment.stencilStoreOp = AttachmentStoreOperator::DONT_CARE;
    depthAttachment.initialLayout = ImageLayout::UNDEFINED;
    depthAttachment.finalLayout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    SubpassDescription subpassDescription = {};
    subpassDescription.colorReferences = {{0, ImageLayout::COLOR_ATTACHMENT_OPTIMAL}};
    subpassDescription.depthStencilReference = {1, ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

//    SubpassDependency subpassDependency = {};
//    subpassDependency.dependencyFlags = DependencyFlagBits::BY_REGION_BIT;
//    subpassDependency.srcSubpass = EG_SUBPASS_EXTERNAL;
//    subpassDependency.dstSubpass = 0;
//    //wait for last image output
//    subpassDependency.srcStageMask = PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT;
//
//    //execute on image output
//    subpassDependency.dstStageMask = PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT;
//
//    //we don't care about access on external subpass
//    subpassDependency.srcAccessMask = 0;
//
//    //we will write on our color attachment
//    subpassDependency.dstAccessMask = AccessFlagBits::COLOR_ATTACHMENT_WRITE_BIT;


    RenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.subpassDescriptions = {subpassDescription};
    renderPassCreateInfo.attachments = {colorAttachment, depthAttachment};

    renderPassCreateInfo.subpassDependencies.resize(2);
    renderPassCreateInfo.subpassDependencies[0].srcSubpass = EG_SUBPASS_EXTERNAL;
    renderPassCreateInfo.subpassDependencies[0].dstSubpass = 0;
    renderPassCreateInfo.subpassDependencies[0].srcStageMask = eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT;
    renderPassCreateInfo.subpassDependencies[0].dstStageMask = eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT;
    renderPassCreateInfo.subpassDependencies[0].srcAccessMask = 0;
    renderPassCreateInfo.subpassDependencies[0].dstAccessMask = eagle::AccessFlagBits::COLOR_ATTACHMENT_READ_BIT | eagle::AccessFlagBits::COLOR_ATTACHMENT_WRITE_BIT;

    renderPassCreateInfo.subpassDependencies[1].srcSubpass = 0;
    renderPassCreateInfo.subpassDependencies[1].dstSubpass = EG_SUBPASS_EXTERNAL;
    renderPassCreateInfo.subpassDependencies[1].srcStageMask = eagle::PipelineStageFlagsBits::COLOR_ATTACHMENT_OUTPUT_BIT;
    renderPassCreateInfo.subpassDependencies[1].dstStageMask = eagle::PipelineStageFlagsBits::FRAGMENT_SHADER_BIT;
    renderPassCreateInfo.subpassDependencies[1].srcAccessMask = eagle::AccessFlagBits::COLOR_ATTACHMENT_WRITE_BIT;
    renderPassCreateInfo.subpassDependencies[1].dstAccessMask = eagle::AccessFlagBits::SHADER_READ_BIT;
    renderPassCreateInfo.subpassDependencies[1].dependencyFlags = eagle::DependencyFlagBits::BY_REGION_BIT;


    VulkanRenderPassCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;

    m_present.renderPass = std::make_shared<VulkanRenderPass>(std::move(renderPassCreateInfo), vulkanCreateInfo);

    EG_TRACE("eagle","Present render pass created!");
}

void VulkanContext::create_framebuffers() {
    EG_TRACE("eagle","Creating present framebuffers!");

    VK_CALL vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &m_present.imageCount, nullptr);
    EG_INFO("eagle","Number of swapchain images: {0}", m_present.imageCount);

    std::vector<VkImage> swapchainImages(m_present.imageCount);

    VK_CALL vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &m_present.imageCount, swapchainImages.data());

    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.width = m_present.extent2D.width;
    imageCreateInfo.height = m_present.extent2D.height;
    imageCreateInfo.format = m_properties.presentFormat;
    imageCreateInfo.aspects = {ImageAspect::COLOR};
    imageCreateInfo.usages = {ImageUsage::COLOR_ATTACHMENT};

    VulkanImageCreateInfo nativeImageCreateInfo = {};
    nativeImageCreateInfo.physicalDevice = m_physicalDevice;
    nativeImageCreateInfo.device = m_device;
    nativeImageCreateInfo.graphicsQueue = m_graphicsQueue;
    nativeImageCreateInfo.commandPool = m_graphicsCommandPool;
    nativeImageCreateInfo.imageCount = m_present.imageCount;

    std::shared_ptr<VulkanImage> colorImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo, swapchainImages);

    //depth
    imageCreateInfo.format = m_properties.depthFormat;
    imageCreateInfo.aspects = {ImageAspect::DEPTH};
    imageCreateInfo.usages = {ImageUsage::DEPTH_STENCIL_ATTACHMENT};
    imageCreateInfo.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    imageCreateInfo.tiling = ImageTiling::OPTIMAL;
    imageCreateInfo.memoryProperties = {MemoryProperty::DEVICE_LOCAL};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    if (imageCreateInfo.format == Format::D32_SFLOAT_S8_UINT || imageCreateInfo.format == Format::D24_UNORM_S8_UINT) {
        imageCreateInfo.aspects.emplace_back(ImageAspect::STENCIL);
        EG_INFO("eagle","Depth format allows stencil aspect");
    }

    std::shared_ptr<VulkanImage> depthImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo);

    FramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.width = m_present.extent2D.width;
    framebufferCreateInfo.height = m_present.extent2D.height;
    framebufferCreateInfo.renderPass = m_present.renderPass;
    framebufferCreateInfo.attachments = {colorImage, depthImage};

    VulkanFramebufferCreateInfo nativeFramebufferCreateInfo = {};
    nativeFramebufferCreateInfo.device = m_device;
    nativeFramebufferCreateInfo.imageCount = m_present.imageCount;

    m_present.framebuffer = std::make_shared<VulkanFramebuffer>(framebufferCreateInfo, nativeFramebufferCreateInfo);

    EG_TRACE("eagle","Framebuffers created!");
}


void VulkanContext::create_command_pool() {

    EG_TRACE("eagle","Creating command pool!");

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

    EG_TRACE("eagle","Command pool created!");
}

void VulkanContext::create_sync_objects() {

    EG_TRACE("eagle","Creating sync objects!");

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

    EG_TRACE("eagle","Sync objects created!");
}


void VulkanContext::recreate_swapchain() {
    EG_TRACE("eagle","Recreating swapchain!");

    VK_CALL
    vkDeviceWaitIdle(m_device);

    //Waits until window is visible
    while (m_window->is_minimized()) {
        m_window->wait_native_events();
    }

    cleanup_swapchain();

    create_swapchain();
    create_framebuffers();
    recreate_objects();

    EG_TRACE("eagle","Swapchain recreated!");
}

void VulkanContext::recreate_objects() {
//    for (auto &shader : m_shaders) {
//        shader->create_pipeline();
//    }
//
//    for (auto &uniformBuffer : m_uniformBuffers) {
//        uniformBuffer->recreate(m_present.imageCount);
//    }
//
//    for (auto& storageBuffer : m_storageBuffers){
//        storageBuffer->recreate(m_present.imageCount);
//    }
//
//    for (auto &descriptorSet : m_descriptorSets) {
//        descriptorSet->recreate(m_present.imageCount);
//    }
//
//    for (auto& computeShader : m_computeShaders){
//        computeShader->recreate(m_present.imageCount);
//    }
//
//    for (auto& commandBuffer : m_commandBuffers){
//        commandBuffer->recreate(m_present.imageCount);
//    }

    context_recreated(this);
}

void VulkanContext::clear_objects() {
    VulkanCleaner::clear();

//    for (auto& commandBuffer : m_commandBuffers){
//        commandBuffer->cleanup();
//    }
//
//    for (auto& computeShader : m_computeShaders){
//        computeShader->clear_descriptor_set();
//    }
//
//    for (auto &descriptorSet : m_descriptorSets) {
//        descriptorSet->cleanup();
//    }
//
//    for (auto &uniformBuffer : m_uniformBuffers) {
//        uniformBuffer->cleanup();
//    }
//
//    for (auto& storageBuffer : m_storageBuffers){
//        storageBuffer->cleanup();
//    }
//
//    for (auto &shader : m_shaders) {
//        shader->cleanup_pipeline();
//    }
}


void VulkanContext::cleanup_swapchain() {
    EG_TRACE("eagle","Clearing swapchain!");

    clear_objects();

    m_present.framebuffer.reset();

    VK_CALL vkDestroySwapchainKHR(m_device, m_present.swapchain, nullptr);
    EG_TRACE("eagle","Swapchain cleared!");
}

std::shared_ptr<Shader>
VulkanContext::create_shader(const ShaderCreateInfo &createInfo) {
    EG_TRACE("eagle","Creating a vulkan shader!");

    VulkanShaderCreateInfo nativeCreateInfo = {};
    nativeCreateInfo.device = m_device;
    nativeCreateInfo.pExtent = &m_present.extent2D;
    return std::make_shared<VulkanShader>(createInfo, nativeCreateInfo);
}

std::shared_ptr<ComputeShader>
VulkanContext::create_compute_shader(const std::string &path) {
    VulkanComputeShaderCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.commandPool = m_computeCommandPool;
    createInfo.imageIndex = &m_present.imageIndex;
    createInfo.bufferCount = m_present.imageCount;
    createInfo.computeQueue = m_computeQueue;
    return std::make_shared<VulkanComputeShader>(path, createInfo);
}

std::shared_ptr<VertexBuffer>
VulkanContext::create_vertex_buffer(const VertexBufferCreateInfo &createInfo, void *data, size_t size) {
    EG_TRACE("eagle","Creating a vulkan vertex buffer!");
    VulkanVertexBufferCreateInfo vulkanCreateInfo = {*m_deleter};
    vulkanCreateInfo.physicalDevice = m_physicalDevice;
    vulkanCreateInfo.device = m_device;
    vulkanCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanCreateInfo.bufferCount = m_present.imageCount;
    return std::make_shared<VulkanVertexBuffer>(createInfo, vulkanCreateInfo, data, size);
}

std::shared_ptr<IndexBuffer>
VulkanContext::create_index_buffer(const IndexBufferCreateInfo &createInfo, void *data, size_t size) {
    EG_TRACE("eagle","Creating a vulkan index buffer!");
    VulkanIndexBufferCreateInfo vulkanCreateInfo = {*m_deleter};
    vulkanCreateInfo.device = m_device;
    vulkanCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanCreateInfo.physicalDevice = m_physicalDevice;
    vulkanCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanCreateInfo.bufferCount = m_present.imageCount;
    return std::make_shared<VulkanIndexBuffer>(createInfo, vulkanCreateInfo, data, size);
}

std::shared_ptr<UniformBuffer>
VulkanContext::create_uniform_buffer(size_t size, void *data) {
    EG_TRACE("eagle","Creating a vulkan uniform buffer!");
    VulkanUniformBufferCreateInfo createInfo = {*m_deleter};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_present.imageCount;
    return std::make_shared<VulkanUniformBuffer>(createInfo, size, data);
}

std::shared_ptr<StorageBuffer>
VulkanContext::create_storage_buffer(size_t size, void *data, UpdateType usage) {
    EG_TRACE("eagle","Creating a vulkan storage buffer!");
    VulkanStorageBufferCreateInfo createInfo = {*m_deleter};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_present.imageCount;
    createInfo.commandPool = m_graphicsCommandPool;
    createInfo.graphicsQueue = m_graphicsQueue;
    return std::make_shared<VulkanStorageBuffer>(createInfo, size, data, usage);
}

std::shared_ptr<DescriptorSetLayout>
VulkanContext::create_descriptor_set_layout(const DescriptorSetLayoutInfo& descriptorSetLayoutInfo) {
    VulkanDescriptorSetLayoutInfo vulkanDescriptorSetLayoutInfo = {};
    vulkanDescriptorSetLayoutInfo.device = m_device;

    return std::make_shared<VulkanDescriptorSetLayout>(descriptorSetLayoutInfo, vulkanDescriptorSetLayoutInfo);
}

std::shared_ptr<DescriptorSet>
VulkanContext::create_descriptor_set(const DescriptorSetInfo& descriptorSetInfo) {
    EG_TRACE("eagle","Creating a vulkan descriptor set!");
    VulkanDescriptorSetInfo vulkanDescriptorSetInfo = {};
    vulkanDescriptorSetInfo.device = m_device;
    vulkanDescriptorSetInfo.bufferCount = m_present.imageCount;

    return std::make_shared<VulkanDescriptorSet>(descriptorSetInfo, vulkanDescriptorSetInfo);
}

std::shared_ptr<Texture>
VulkanContext::create_texture(const TextureCreateInfo &createInfo) {

    EG_TRACE("eagle","Creating a vulkan texture!");
    VulkanTextureCreateInfo vulkanTextureCreateInfo = {};
    vulkanTextureCreateInfo.device = m_device;
    vulkanTextureCreateInfo.physicalDevice = m_physicalDevice;
    vulkanTextureCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanTextureCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanTextureCreateInfo.imageCount = m_present.imageCount;

    return std::make_shared<VulkanTexture>(createInfo, vulkanTextureCreateInfo);
}

std::shared_ptr<Image>
VulkanContext::create_image(const ImageCreateInfo &createInfo) {
    EG_TRACE("eagle","Creating a vulkan image!");
    VulkanImageCreateInfo vulkanImageCreateInfo = {};
    vulkanImageCreateInfo.device = m_device;
    vulkanImageCreateInfo.physicalDevice = m_physicalDevice;
    vulkanImageCreateInfo.commandPool = m_graphicsCommandPool;
    vulkanImageCreateInfo.graphicsQueue = m_graphicsQueue;
    vulkanImageCreateInfo.imageCount = m_present.imageCount;

    return std::make_shared<VulkanImage>(createInfo, vulkanImageCreateInfo);
}

std::shared_ptr <RenderPass> VulkanContext::create_render_pass(RenderPassCreateInfo createInfo) {
    VulkanRenderPassCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;

    return std::make_shared<VulkanRenderPass>(createInfo, vulkanCreateInfo);
}

std::shared_ptr<Framebuffer> VulkanContext::create_framebuffer(const FramebufferCreateInfo &createInfo) {

    VulkanFramebufferCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;
    vulkanCreateInfo.imageCount = m_present.imageCount;

    return std::make_shared<VulkanFramebuffer>(createInfo, vulkanCreateInfo);
}

std::shared_ptr<CommandBuffer> VulkanContext::create_command_buffer(const CommandBufferCreateInfo& createInfo) {

    QueueFamilyIndices queueFamilyIndices = find_family_indices(m_physicalDevice);

    VulkanCommandBufferCreateInfo vkCreateInfo = {};
    vkCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    vkCreateInfo.device = m_device;
    vkCreateInfo.imageCount = m_present.imageCount;
    vkCreateInfo.currentImageIndex = &m_present.imageIndex;
    return std::make_shared<VulkanCommandBuffer>(createInfo, vkCreateInfo);
}

const RenderingContext::Properties& VulkanContext::properties() {
    return m_properties;
}

bool VulkanContext::prepare_frame(Semaphore* signalAvailableImage) {

    auto semaphore = dynamic_cast<VulkanSemaphore*>(signalAvailableImage);

    VkResult result;
    VK_CALL
    result = vkAcquireNextImageKHR(
            m_device,
            m_present.swapchain,
            std::numeric_limits<uint64_t>::max(),
            semaphore->native_semaphore(m_currentFrame),
            VK_NULL_HANDLE,
            &m_present.imageIndex);

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

void VulkanContext::present_frame() {

    //submit command buffer
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = m_present.commandBuffers.size();
        submitInfo.pCommandBuffers = m_present.commandBuffers.data();

        VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VK_CALL
        vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

        VkResult result = vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to submit command buffer!");
        }
        m_present.commandBuffers.clear();
    }

    //present
    {
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
}

void VulkanContext::destroy_texture_2d(const std::shared_ptr<Texture> &texture) {
    std::shared_ptr<VulkanTexture> t = std::static_pointer_cast<VulkanTexture>(texture);
//    m_textures.erase(std::find(m_textures.begin(), m_textures.end(), t));
}

std::shared_ptr<RenderPass> VulkanContext::main_render_pass() {
    return m_present.renderPass;
}

std::shared_ptr<Framebuffer> VulkanContext::main_framebuffer() {
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

    auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkCreateDebugUtilsMessengerEXT != nullptr) {
        return vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pCallback);
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


}
