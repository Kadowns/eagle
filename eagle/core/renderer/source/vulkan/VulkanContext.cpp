//
// Created by Novak on 08/05/2019.
//

#include <set>

#include "eagle/core/renderer/vulkan/VulkanContext.h"
#include "eagle/core/renderer/vulkan/VulkanHelper.h"

#include "eagle/core/Window.h"
#include "eagle/core/Log.h"

#include <GLFW/glfw3.h>
#include <eagle/Eagle.h>


EG_BEGIN

bool VulkanContext::enableValidationLayers = true;

VulkanContext::VulkanContext() { // NOLINT(cppcoreguidelines-pro-type-member-init)
    EAGLE_SET_INFO(EAGLE_RENDERING_CONTEXT, "Vulkan");
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::init(Window *window) {
    EG_CORE_TRACE("Initializing vulkan context!");

    m_window = window;

    create_instance();
    create_debug_callback();
    create_surface();
    bind_physical_device();
    create_logical_device();
    create_sync_objects();

    create_swapchain();
    create_swapchain_images();

    create_command_pool();
    allocate_command_buffers();

    create_render_pass();
    create_offscreen_render_pass();
    create_depth_resources();
    create_framebuffers();

    EG_CORE_TRACE("Vulkan ready!");
}

void VulkanContext::deinit() {

    EG_CORE_TRACE("Terminating vulkan!");

    VK_CALL
    vkDeviceWaitIdle(m_device);

    m_vertexBuffers.clear();
    m_dirtyVertexBuffers.clear();
    m_indexBuffers.clear();
    m_dirtyIndexBuffers.clear();

    for (auto& renderTarget : m_renderTargets){
        renderTarget->cleanup();
    }

    cleanup_swapchain();
    m_descriptorSetsLayouts.clear();
    m_descriptorSets.clear();
    m_dirtyUniformBuffers.clear();
    m_uniformBuffers.clear();
    m_textures.clear();
    m_renderTargets.clear();
    m_shaders.clear();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_CALL
        vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
        VK_CALL
        vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
        VK_CALL
        vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
    }

    VK_CALL
    vkDestroyCommandPool(m_device, m_commandPool, nullptr);

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
    appInfo.pApplicationName = EAGLE_GET_INFO(EAGLE_APP_NAME).c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "eagle";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = get_required_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
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
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
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

void VulkanContext::create_surface() {

    EG_CORE_TRACE("Creating window surface!");
    VK_CALL_ASSERT(
            glfwCreateWindowSurface(m_instance, (GLFWwindow *) m_window->get_native_window(), nullptr, &m_surface)) {
        throw std::runtime_error("failed to create window surface!");
    }
    EG_CORE_TRACE("Window surface created!");
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

    //samplers
    if (!deviceFeatures.samplerAnisotropy) {
        return 0;
    }

    //se n�o tiver geometry shader, pula fora
    if (!deviceFeatures.geometryShader) {
        return 0;
    }

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

std::vector<const char *> VulkanContext::get_required_extensions() {
    EG_CORE_TRACE("Getting required extensions!");
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}


VkBool32 VulkanContext::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    VkDebugInfo::VkCall info = *((VkDebugInfo::VkCall *) pUserData);

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

        if (indices.isComplete()) {
            break;
        }
        i++;
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
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

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

    VK_CALL
    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    VK_CALL
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

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
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    //Informa��es da swapchain
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;//VK_IMAGE_USAGE_TRANSFER_DST_BIT para post process
    //-----------------------------

    QueueFamilyIndices indices = find_family_indices(m_physicalDevice);
    uint32_t queueFamilyIndices[] = {(uint32_t) indices.graphicsFamily.value(),
                                     (uint32_t) indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
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

    //query da quantidade de imagens
    VK_CALL
    vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &imageCount, nullptr);
    m_present.swapchainImages.resize(imageCount);

    EG_CORE_INFO_F("Number of swapchain images: {0}", m_present.swapchainImages.size());

    //efetivamente pega as imagens
    VK_CALL
    vkGetSwapchainImagesKHR(m_device, m_present.swapchain, &imageCount, m_present.swapchainImages.data());
    EG_CORE_TRACE("Swapchain created!");
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
        actualExtent.width = std::clamp<uint32_t>(m_window->get_width(), capabilities.minImageExtent.width,
                                                  capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp<uint32_t>(m_window->get_height(), capabilities.minImageExtent.height,
                                                   capabilities.maxImageExtent.height);

        EG_CORE_INFO_F("Swap extent: width:{0} height:{1}", actualExtent.width, actualExtent.height);
        return actualExtent;
    }
}

void VulkanContext::create_swapchain_images() {
    EG_CORE_INFO("Create swapchain images!");

    m_present.swapchainImageViews.resize(m_present.swapchainImages.size());

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    for (size_t i = 0; i < m_present.swapchainImageViews.size(); i++) {
        VulkanHelper::create_image_view(
                m_device,
                m_present.swapchainImages[i],
                m_present.swapchainImageViews[i],
                m_present.swapchainFormat,
                VK_IMAGE_VIEW_TYPE_2D,
                subresourceRange
        );
    }

    EG_CORE_INFO("Swapchain images created!");
}

void VulkanContext::handle_window_resized(int width, int height) {
    m_windowResized = true;
}

void VulkanContext::create_depth_resources() {
    VkFormat depthFormat = find_depth_format();

    VulkanHelper::create_image(m_physicalDevice, m_device,
                               m_present.extent2D.width, m_present.extent2D.height, 1, 1,
                               depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_present.depth.image, m_present.depth.memory);

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    auto has_stencil_component = [&](VkFormat format) {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    };

    if (has_stencil_component(depthFormat)) {
        subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }

    VulkanHelper::create_image_view(m_device, m_present.depth.image, m_present.depth.view, depthFormat,
                                    VK_IMAGE_VIEW_TYPE_2D, subresourceRange);


    VulkanHelper::transition_image_layout(m_device, m_commandPool, m_graphicsQueue,
                                          m_present.depth.image, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
                                          VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, subresourceRange);
}

VkFormat VulkanContext::find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                              VkFormatFeatureFlags features) {
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

VkFormat VulkanContext::find_depth_format() {
    return find_supported_format({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void VulkanContext::create_render_pass() {

    EG_CORE_TRACE("Creating render pass!");
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
    depthAttachment.format = find_depth_format();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkSubpassDependency, 2> dependencies = {};
    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = 0;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;


    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachments.size();
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = dependencies.size();
    renderPassInfo.pDependencies = dependencies.data();

    VK_CALL_ASSERT(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_present.renderPass)) {
        throw std::runtime_error("failed to create render pass!");
    }

    EG_CORE_TRACE("Render pass created!");
}

void VulkanContext::create_command_pool() {

    EG_CORE_TRACE("Creating command pool!");

    QueueFamilyIndices queueFamilyIndices = find_family_indices(m_physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VK_CALL_ASSERT(vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool)) {
        throw std::runtime_error("failed to create command pool!");
    }

    EG_CORE_TRACE("Command pool created!");
}

void VulkanContext::create_framebuffers() {

    EG_CORE_TRACE("Creating framebuffers!");

    m_present.framebuffers.resize(m_present.swapchainImages.size());

    EG_CORE_INFO_F("Number of framebuffers: {0}", m_present.framebuffers.size());

    for (size_t i = 0; i < m_present.swapchainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
                m_present.swapchainImageViews[i],
                m_present.depth.view
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_present.renderPass;
        framebufferInfo.attachmentCount = attachments.size();
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_present.extent2D.width;
        framebufferInfo.height = m_present.extent2D.height;
        framebufferInfo.layers = 1;

        VK_CALL_ASSERT(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_present.framebuffers[i])) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    EG_CORE_TRACE("Framebuffers created!");
}

void VulkanContext::allocate_command_buffers() {

    EG_CORE_TRACE("Allocating primary command buffers!");

    m_commandBuffers.resize(m_present.swapchainImages.size());
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) m_commandBuffers.size();

    VK_CALL_ASSERT(vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data())) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    EG_CORE_TRACE("Primary command buffers allocated!");
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
    create_swapchain_images();
    allocate_command_buffers();
    create_render_pass();
    create_offscreen_render_pass();
    create_depth_resources();
    create_framebuffers();

    for (auto& renderTarget : m_renderTargets){
//        renderTarget->create(m_present.extent2D.width, m_present.extent2D.height);
    }

    for (auto &shader : m_shaders) {
        shader->create_pipeline();
    }

    for (auto &uniformBuffer : m_uniformBuffers) {
        uniformBuffer->create_uniform_buffer();
    }

    for (auto &descriptorSet : m_descriptorSets) {
        descriptorSet->create_descriptor_pool();
        descriptorSet->create_descriptor_sets();
    }

    EG_CORE_TRACE("Swapchain recreated!");
}

void VulkanContext::cleanup_swapchain() {
    EG_CORE_TRACE("Clearing swapchain!");

    for (size_t i = 0; i < m_present.framebuffers.size(); i++) {
        VK_CALL vkDestroyFramebuffer(m_device, m_present.framebuffers[i], nullptr);
    }

    VK_CALL vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()),
                         m_commandBuffers.data());

    for (auto &descriptorSet : m_descriptorSets) {
        descriptorSet->cleanup();
    }

    for (auto &uniformBuffer : m_uniformBuffers) {
        uniformBuffer->cleanup();
    }

    for (auto &shader : m_shaders) {
        shader->cleanup_pipeline();
    }

    VK_CALL vkDestroyRenderPass(m_device, m_present.renderPass, nullptr);
    VK_CALL vkDestroyRenderPass(m_device, m_present.offscreenPass, nullptr);

    for (size_t i = 0; i < m_present.swapchainImageViews.size(); i++) {
        VK_CALL vkDestroyImageView(m_device, m_present.swapchainImageViews[i], nullptr);
    }

    VK_CALL vkDestroyImageView(m_device, m_present.depth.view, nullptr);
    VK_CALL vkDestroyImage(m_device, m_present.depth.image, nullptr);
    VK_CALL vkFreeMemory(m_device, m_present.depth.memory, nullptr);

    VK_CALL vkDestroySwapchainKHR(m_device, m_present.swapchain, nullptr);
    EG_CORE_TRACE("Swapchain cleared!");
}

Handle<Shader>
VulkanContext::create_shader(const std::string &vertFilePath, const std::string &fragFilePath,
                             const std::vector<Reference<DescriptorSetLayout>> &descriptorSetLayouts,
                             const ShaderPipelineInfo &pipelineInfo) {
    EG_CORE_TRACE("Creating a vulkan shader!");

    std::vector<Reference<VulkanDescriptorSetLayout>> vkDescriptorSetLayouts(descriptorSetLayouts.size());
    for (uint32_t i = 0; i < descriptorSetLayouts.size(); i++){
        vkDescriptorSetLayouts[i] = std::static_pointer_cast<VulkanDescriptorSetLayout>(descriptorSetLayouts[i]);
    }


    VulkanShader::VulkanShaderCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.pExtent = &m_present.extent2D;
    createInfo.pRenderPass = pipelineInfo.offscreenRendering ? &m_present.offscreenPass : &m_present.renderPass;
    m_shaders.emplace_back(std::make_shared<VulkanShader>(vertFilePath, fragFilePath, vkDescriptorSetLayouts, pipelineInfo, createInfo));
    return m_shaders.back();
}

Handle<VertexBuffer>
VulkanContext::create_vertex_buffer(void *vertices, uint32_t count, const VertexLayout &vertexLayout,
                                    EG_BUFFER_USAGE usage) {
    EG_CORE_TRACE("Creating a vulkan vertex buffer!");
    VulkanVertexBufferCreateInfo createInfo(vertexLayout);
    createInfo.data = vertices;
    createInfo.count = count;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_commandPool;
    createInfo.graphicsQueue = m_graphicsQueue;
    createInfo.bufferCount = usage == EG_BUFFER_USAGE::DYNAMIC ? m_present.swapchainImages.size() : 1;
    m_vertexBuffers.emplace_back(std::make_shared<VulkanVertexBuffer>(m_device, createInfo, usage));
    return m_vertexBuffers.back();
}

Handle<IndexBuffer>
VulkanContext::create_index_buffer(void *indexData, size_t indexCount, INDEX_BUFFER_TYPE indexType,
                                   EG_BUFFER_USAGE usage) {
    EG_CORE_TRACE("Creating a vulkan index buffer!");
    VulkanIndexBufferCreateInfo createInfo = {};
    createInfo.graphicsQueue = m_graphicsQueue;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_commandPool;
    createInfo.bufferCount = usage == EG_BUFFER_USAGE::DYNAMIC ? m_present.swapchainImages.size() : 1;
    m_indexBuffers.emplace_back(std::make_shared<VulkanIndexBuffer>(m_device, createInfo, indexData, indexCount, indexType, usage));
    return m_indexBuffers.back();
}

Handle<UniformBuffer>
VulkanContext::create_uniform_buffer(size_t size, void *data) {
    EG_CORE_TRACE("Creating a vulkan uniform buffer!");
    VulkanUniformBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_present.swapchainImages.size();
    m_uniformBuffers.emplace_back(std::make_shared<VulkanUniformBuffer>(createInfo, size, data));
    return m_uniformBuffers.back();
}


Handle<DescriptorSetLayout>
VulkanContext::create_descriptor_set_layout(const std::vector<DescriptorBinding> &bindings) {
    m_descriptorSetsLayouts.emplace_back(std::make_shared<VulkanDescriptorSetLayout>(m_device, bindings));
    return m_descriptorSetsLayouts.back();
}

Handle<DescriptorSet>
VulkanContext::create_descriptor_set(const Reference<DescriptorSetLayout>& descriptorLayout,
                                     const std::vector<Reference<DescriptorItem>> &descriptorItems) {
    EG_CORE_TRACE("Creating a vulkan descriptor set!");
    VulkanDescriptorSetCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.bufferCount = m_present.swapchainImages.size();

    m_descriptorSets.emplace_back(std::make_shared<VulkanDescriptorSet>(
            std::static_pointer_cast<VulkanDescriptorSetLayout>(descriptorLayout),
            descriptorItems,
            createInfo
            )
    );
    return m_descriptorSets.back();
}

Handle<Texture2D>
VulkanContext::create_texture_2d(Texture2DCreateInfo &createInfo) {

    EG_CORE_TRACE("Creating a vulkan texture!");
    VulkanTexture2DCreateInfo vulkanTextureCreateInfo = {};
    vulkanTextureCreateInfo.device = m_device;
    vulkanTextureCreateInfo.physicalDevice = m_physicalDevice;
    vulkanTextureCreateInfo.commandPool = m_commandPool;
    vulkanTextureCreateInfo.graphicsQueue = m_graphicsQueue;

    m_textures.emplace_back(std::make_shared<VulkanTexture2D>(createInfo, vulkanTextureCreateInfo));
    return m_textures.back();
}

Handle<RenderTarget>
VulkanContext::create_render_target(const std::vector<RENDER_TARGET_ATTACHMENT> &attachments) {

    VulkanRenderTargetCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.depthFormat = find_depth_format();
    createInfo.colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.renderPass = m_present.offscreenPass;
    m_renderTargets.emplace_back(std::make_shared<VulkanRenderTarget>(m_present.extent2D.width, m_present.extent2D.height, attachments, createInfo));
    return m_renderTargets.back();
}

void
VulkanContext::uniform_buffer_flush(const Reference<UniformBuffer> &uniformBuffer, void *data) {

    Reference<VulkanUniformBuffer> vulkanUniformBuffer = std::static_pointer_cast<VulkanUniformBuffer>(
            uniformBuffer);
    vulkanUniformBuffer->upload_data(data);
    if (std::find(m_dirtyUniformBuffers.begin(), m_dirtyUniformBuffers.end(), vulkanUniformBuffer) == m_dirtyUniformBuffers.end()) {
        m_dirtyUniformBuffers.emplace_back(vulkanUniformBuffer);
    }
}

void
VulkanContext::vertex_buffer_flush(const Reference<VertexBuffer> &vertexBuffer, void *data, uint32_t vertexCount){
    Reference<VulkanVertexBuffer> vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);
    vulkanVertexBuffer->upload(data, vertexCount);
    if (std::find(m_dirtyVertexBuffers.begin(), m_dirtyVertexBuffers.end(), vulkanVertexBuffer) == m_dirtyVertexBuffers.end()) {
        m_dirtyVertexBuffers.emplace_back(vulkanVertexBuffer);
    }
}

void
VulkanContext::index_buffer_flush(const Reference<IndexBuffer> &indexBuffer, void *data, uint32_t indexCount) {
    Reference<VulkanIndexBuffer> vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);
    vulkanIndexBuffer->upload(data, indexCount);
    if (std::find(m_dirtyIndexBuffers.begin(), m_dirtyIndexBuffers.end(), vulkanIndexBuffer) == m_dirtyIndexBuffers.end()) {
        m_dirtyIndexBuffers.emplace_back(vulkanIndexBuffer);
    }
}

void VulkanContext::bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) {
    if (!m_drawInitialized) {
        EG_CORE_ERROR("Bind vertex buffer called outside of draw range!");
        return;
    }
    Reference<VulkanVertexBuffer> vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);

    m_commandList.push(std::make_shared<VulkanCommandBindVertexBuffer>(
            m_present.commandBuffer,
            vulkanVertexBuffer->get_buffer(m_drawInfo.imageIndex).get_native_buffer()
            ));
}

void VulkanContext::bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) {
    if (!m_drawInitialized) {
        EG_CORE_ERROR("Bind index buffer called outside of draw range!");
        return;
    }
    Reference<VulkanIndexBuffer> vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);

    m_commandList.push(std::make_shared<VulkanCommandBindIndexBuffer>(
            m_present.commandBuffer,
            vulkanIndexBuffer->get_buffer(m_drawInfo.imageIndex).get_native_buffer(),
            vulkanIndexBuffer->get_native_index_type()
    ));
}

void
VulkanContext::draw(uint32_t vertexCount) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("Draw called outside of draw range!");
        return;
    }

    m_commandList.push(std::make_shared<VulkanCommandDraw>(m_present.commandBuffer, vertexCount));
}

void
VulkanContext::bind_shader(const Reference<Shader> &shader) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("Bind shader called outside of draw range!");
        return;
    }

    Reference<VulkanShader> vulkanShader = std::static_pointer_cast<VulkanShader>(shader);
    m_commandList.push(std::make_shared<VulkanCommandBindShader>(m_present.commandBuffer, vulkanShader->get_pipeline()));
}

void
VulkanContext::draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) {
    if (!m_drawInitialized) {
        EG_CORE_ERROR("Draw indexed called outside of draw range!");
        return;
    }

    m_commandList.push(std::make_shared<VulkanCommandDrawIndexed>(m_present.commandBuffer, indicesCount, indexOffset, vertexOffset));
}

void
VulkanContext::bind_descriptor_sets(const Reference<Shader> &shader, const Reference<DescriptorSet> &descriptorSet,
                                    uint32_t setIndex) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("bind descriptor called outside of draw range!");
        return;
    }


    auto vkDescriptor = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet)->get_descriptors()[m_drawInfo.imageIndex];

    m_commandList.push(std::make_shared<VulkanCommandBindDescriptorSet>(
            m_present.commandBuffer,
            std::static_pointer_cast<VulkanShader>(shader)->get_layout(),
            vkDescriptor,
            setIndex
            ));
}

void
VulkanContext::push_constants(const Reference<Shader> &shader, EG_SHADER_STAGE stage, uint32_t offset, size_t size,
                              void *data) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("Push constants called outside of draw range!");
        return;
    }

    VkShaderStageFlags shaderStage;
    switch(stage){
        case EG_SHADER_STAGE::VERTEX: shaderStage = VK_SHADER_STAGE_VERTEX_BIT; break;
        case EG_SHADER_STAGE::FRAGMENT: shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
        case EG_SHADER_STAGE::COMPUTE: shaderStage = VK_SHADER_STAGE_COMPUTE_BIT; break;
        case EG_SHADER_STAGE::GEOMETRY: shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT; break;
        case EG_SHADER_STAGE::TESSALATION_CONTROL: shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
        case EG_SHADER_STAGE::TESSALATION_EVALUATE: shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
    }

    auto vulkanShader = std::static_pointer_cast<VulkanShader>(shader);
    m_commandList.push(std::make_shared<VulkanCommandPushConstants>(m_present.commandBuffer, vulkanShader->get_layout(), shaderStage, offset, size, data));

}

bool VulkanContext::begin_draw_commands() {

    VK_CALL
    vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result;
    VK_CALL
    result = vkAcquireNextImageKHR(m_device, m_present.swapchain, std::numeric_limits<uint64_t>::max(),
                                   m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_drawInfo.imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swapchain image!");
    }

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    //get the correct commandbuffer for this layer (current image + layer offset in the commandbuffer vector)
    m_present.commandBuffer = m_commandBuffers[m_drawInfo.imageIndex];


    VK_CALL_ASSERT(vkBeginCommandBuffer(m_commandBuffers[m_drawInfo.imageIndex], &beginInfo)) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }
    m_drawInitialized = true;

    //updates dirty buffers-------------------------
    if (!m_dirtyUniformBuffers.empty()) {
        std::vector<Reference<VulkanUniformBuffer>> dirtyBuffers;
        for (auto &buffer : m_dirtyUniformBuffers) {
            buffer->flush(m_drawInfo.imageIndex);

            //if the buffer is still dirty, we want to maintain it on the dirt buffers vector
            if (buffer->is_dirty()) {
                dirtyBuffers.emplace_back(buffer);
            }
        }
        //swaps the old buffer vector with the new one
        std::swap(m_dirtyUniformBuffers, dirtyBuffers);
    }


    //vertex buffers
    if (!m_dirtyVertexBuffers.empty()){
        std::vector<Reference<VulkanVertexBuffer>> dirtyBuffers;
        for (auto& buffer : m_dirtyVertexBuffers) {
            buffer->flush(m_drawInfo.imageIndex);
            if (buffer->is_dirty()){
                dirtyBuffers.emplace_back(buffer);
            }
        }
        std::swap(m_dirtyVertexBuffers, dirtyBuffers);
    }

    //index buffers
    if (!m_dirtyIndexBuffers.empty()){
        std::vector<Reference<VulkanIndexBuffer>> dirtyBuffers;
        for (auto& buffer : m_dirtyIndexBuffers) {
            buffer->flush(m_drawInfo.imageIndex);
            if (buffer->is_dirty()){
                dirtyBuffers.emplace_back(buffer);
            }
        }
        std::swap(m_dirtyIndexBuffers, dirtyBuffers);
    }
    //-------------------------------------------------------

    return true;
}

void VulkanContext::end_draw_commands() {

    //push a begin render pass command just before all onscreen rendering commands
    m_commandList.begin_secondary_pass();

    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    m_commandList.push(std::make_shared<VulkanCommandBeginRenderPass>(
            m_present.commandBuffer,
            m_present.renderPass,
            m_present.framebuffers[m_drawInfo.imageIndex],
            m_present.extent2D,
            clearValues
    ));

    m_commandList.end_secondary_pass();

    //push a end render pass command after all commands
    m_commandList.push(std::make_shared<VulkanCommandEndRenderPass>(m_present.commandBuffer));


    //registers all commands on command buffer
    m_commandList.execute();

    m_commandList.clear();

    VK_CALL_ASSERT(vkEndCommandBuffer(m_present.commandBuffer)) {
        throw std::runtime_error("failed to record command buffer!");
    }

    m_drawInitialized = false;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[m_drawInfo.imageIndex];

    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    VK_CALL vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

    VK_CALL_ASSERT(vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame])) {
        throw std::runtime_error("failed to submit command buffer!");
    }
}

void VulkanContext::begin_draw_offscreen(const Reference<RenderTarget> &renderTarget) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("Begin draw offscreen called outside of draw range!");
        return;
    }

    auto vkRenderTarget = std::static_pointer_cast<VulkanRenderTarget>(renderTarget);

    m_commandList.begin_secondary_pass();

    m_commandList.push(std::make_shared<VulkanCommandBeginRenderPass>(
            m_present.commandBuffer,
            m_present.offscreenPass,
            vkRenderTarget->get_framebuffer(),
            vkRenderTarget->get_extent(),
            vkRenderTarget->get_clear_values()
            ));
}

void VulkanContext::end_draw_offscreen() {
    if (!m_drawInitialized) {
        EG_CORE_ERROR("End draw offscreen called outside of draw range!");
        return;
    }

    m_commandList.push(std::make_shared<VulkanCommandEndRenderPass>(m_present.commandBuffer));
    m_commandList.end_secondary_pass();
}

void VulkanContext::refresh() {

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore waitSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapChains[] = {m_present.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_drawInfo.imageIndex;

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

void VulkanContext::set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) {
    if (!m_drawInitialized) {
        EG_CORE_ERROR("Set viewport called outside of draw range!");
        return;
    }

    VkViewport viewport = {};
    viewport.x = x;
    viewport.y = y;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = minDepth;
    viewport.maxDepth = maxDepth;
    m_commandList.push(std::make_shared<VulkanCommandSetViewport>(m_present.commandBuffer, viewport));
}

void VulkanContext::set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) {

    if (!m_drawInitialized) {
        EG_CORE_ERROR("Set scissor called outside of draw range!");
        return;
    }

    VkRect2D scissor = {};
    scissor.offset.x = x;
    scissor.offset.y = y;
    scissor.extent.width = w;
    scissor.extent.height = h;
    m_commandList.push(std::make_shared<VulkanCommandSetScissor>(m_present.commandBuffer, scissor));

}

void VulkanContext::create_offscreen_render_pass() {
    // + 1 for depth attachment
    std::array<VkAttachmentDescription, 2> attachmentDescriptions = {};
    // Color attachment

    attachmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM;
    attachmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescriptions[1].format = find_depth_format();
    attachmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::vector<VkSubpassDependency> dependencies(attachmentDescriptions.size());
    for (size_t i = 0; i < dependencies.size(); i++){

        dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        if (i != dependencies.size() - 1){
            //color
            dependencies[i].srcSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[i].dstSubpass = 0;
            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependencies[i].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dependencies[i].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        } else {
            //depth
            dependencies[i].srcSubpass = 0;
            dependencies[i].dstSubpass = VK_SUBPASS_EXTERNAL;
            dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependencies[i].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        }
    }


    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
    renderPassInfo.pAttachments = attachmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();

    VK_CALL vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_present.offscreenPass);
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
