//
// Created by Novak on 08/05/2019.
//

#include <set>

#include "eagle/renderer/vulkan/VulkanContext.h"
#include "eagle/renderer/vulkan/VulkanHelper.h"

#include "eagle/core/Window.h"
#include "eagle/core/Log.h"

#include <GLFW/glfw3.h>


_EAGLE_BEGIN

bool VulkanContext::enableValidationLayers = true;

VulkanContext::VulkanContext() { // NOLINT(cppcoreguidelines-pro-type-member-init)
    EAGLE_SET_INFO(EAGLE_RENDERING_CONTEXT, "Vulkan");
}

VulkanContext::~VulkanContext() = default;

void VulkanContext::init(Window* window) {
    EG_CORE_TRACE("Initializing vulkan context!");

    m_window = window;

    create_instance();
    create_debug_callback();
    create_surface();
    bind_physical_device();
    create_logical_device();
    create_swapchain();
    create_swapchain_images();
    create_render_pass();
    create_framebuffers();
    create_command_pool();
    allocate_command_buffers();
    create_sync_objects();

    EG_CORE_TRACE("Vulkan ready!");
}

void VulkanContext::deinit() {

    EG_CORE_TRACE("Terminating vulkan!");

    VK_CALL vkDeviceWaitIdle(m_device);

    m_vertexBuffers.clear();
    m_indexBuffers.clear();

    cleanup_swapchain();
    m_descriptorSets.clear();
    m_dirtyUniformBuffers.clear();
    m_uniformBuffers.clear();
    m_textures.clear();
    m_shaders.clear();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VK_CALL vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
        VK_CALL vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
        VK_CALL vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
    }

    VK_CALL vkDestroyCommandPool(m_device, m_commandPool, nullptr);

    VK_CALL vkDestroyDevice(m_device, nullptr);

    VK_CALL vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (enableValidationLayers) {
        VK_CALL DestroyDebugUtilsMessengerEXT(m_instance, m_debugMessenger, nullptr);
    }

    VK_CALL vkDestroyInstance(m_instance, nullptr);

    EG_CORE_TRACE("Vulkan terminated!");
}


void VulkanContext::begin_draw() {
    m_drawInfo.invalidFrame = false;

    VK_CALL vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result;
    VK_CALL result = vkAcquireNextImageKHR(m_device, m_swapchain, std::numeric_limits<uint64_t>::max(), m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_drawInfo.imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR){
        recreate_swapchain();
        m_drawInfo.invalidFrame = true;
        return;
    } else if (result != VK_SUCCESS  && result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("failed to acquire swapchain image!");
    }


    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK_CALL_ASSERT(vkBeginCommandBuffer(m_commandBuffers[m_drawInfo.imageIndex], &beginInfo)) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_framebuffers[m_drawInfo.imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapchainExtent;

    VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    VK_CALL vkCmdBeginRenderPass(m_commandBuffers[m_drawInfo.imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    m_drawInitialized = true;
}

void VulkanContext::end_draw() {
    if (m_drawInfo.invalidFrame){
        return;
    }

    VK_CALL vkCmdEndRenderPass(m_commandBuffers[m_drawInfo.imageIndex]);

    //updates dirty buffers
    if (!m_dirtyUniformBuffers.empty()){
        std::vector<std::shared_ptr<VulkanUniformBuffer>> dirtyBuffers;
        for (auto& buffer : m_dirtyUniformBuffers){
            buffer->flush(m_drawInfo.imageIndex);

            //if the buffer is still dirty, we want to maintain it on the dirt buffers vector
            if (buffer->is_dirty()){
                dirtyBuffers.emplace_back(buffer);
            }
        }

        //swaps the old buffer vector with the new one
        std::swap(m_dirtyUniformBuffers, dirtyBuffers);
    }

    m_drawInitialized = false;

    VK_CALL_ASSERT(vkEndCommandBuffer(m_commandBuffers[m_drawInfo.imageIndex])) {
        throw std::runtime_error("failed to record command buffer!");
    }

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
        throw std::runtime_error("failed to submit draw_vertex_buffer command buffer!");
    }
}


void VulkanContext::refresh() {
    if (m_drawInfo.invalidFrame){
        return;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore waitSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_drawInfo.imageIndex;

    VK_CALL VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowResized){
        m_windowResized = false;
        recreate_swapchain();

    } else if (result != VK_SUCCESS){
        throw std::runtime_error("failed to present swapchain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
    appInfo.pEngineName = "Eagle";
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
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debug_callback;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
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
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debug_callback;
    createInfo.pUserData = &VkDebugInfo::get_last_call();

    VK_CALL_ASSERT(CreateDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugMessenger)){
        throw std::runtime_error("failed to setup debug callback!");
    }
    EG_CORE_TRACE("Debug callback ready!");
}

void VulkanContext::create_surface() {

    EG_CORE_TRACE("Creating window surface!");
    VK_CALL_ASSERT(glfwCreateWindowSurface( m_instance, (GLFWwindow*) m_window->get_native_window(), nullptr, &m_surface)){
        throw std::runtime_error("failed to create window surface!");
    }
    EG_CORE_TRACE("Window surface created!");
}

void VulkanContext::bind_physical_device() {

    EG_CORE_TRACE("Selecting physical device!");

    //verifica quantas placas de video suportam a vulkan no pc
    uint32_t deviceCount = 0;
    VK_CALL vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    EG_CORE_INFO_F("Number of supporting devices: {0}", deviceCount);

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

    EG_CORE_TRACE("Physical device selected!");
}

int VulkanContext::evaluate_device(VkPhysicalDevice device) {

    EG_CORE_TRACE("Evaluating device!");
    VkPhysicalDeviceProperties physicalDeviceProperties = {};

    VK_CALL vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
    //features
    VkPhysicalDeviceFeatures deviceFeatures = {};
    VK_CALL vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    //samplers
    if (!deviceFeatures.samplerAnisotropy){
        return 0;
    }

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


bool VulkanContext::validation_layers_supported(){
    EG_CORE_TRACE("Checking validation layer support!");
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
    EG_CORE_TRACE("Getting required extensions!");
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

    VkDebugInfo::VkCall info = *((VkDebugInfo::VkCall*)pUserData);

    switch(messageSeverity){
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
            Log::core_log(info.fileName.c_str(), info.funcName.c_str(), info.line, Log::CRITICAL, pCallbackData->pMessage);
            break;
    }
    return VK_FALSE;
}

VulkanContext::QueueFamilyIndices VulkanContext::find_family_indices(VkPhysicalDevice device) {
    EG_CORE_TRACE("Finding queue family indices!");
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

    EG_CORE_TRACE("Checking device extensions!");

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

    EG_CORE_TRACE("Querying swapchain support!");
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

    VK_CALL_ASSERT(vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device)){
        throw std::runtime_error("failed to create logical device!");
    }

    VK_CALL vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    VK_CALL vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);

    EG_CORE_TRACE("Logical device created!");
}

VkSurfaceFormatKHR VulkanContext::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats) {

    if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        EG_CORE_INFO("Swapchain surface format: VK_FORMAT_UNDEFINED");
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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

    m_swapchainFormat = surfaceFormat.format;
    m_swapchainExtent = extent;

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
    uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily.value(), (uint32_t)indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CALL_ASSERT(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain)) {
        throw std::runtime_error("failed to create swap chain!");
    }

    //query da quantidade de imagens
    VK_CALL vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    m_swapchainImages.resize(imageCount);

    EG_CORE_INFO_F("Number of swapchain images: {0}", m_swapchainImages.size());

    //efetivamente pega as imagens
    VK_CALL vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());
    EG_CORE_TRACE("Swapchain created!");
}

VkPresentModeKHR VulkanContext::choose_swap_present_mode(const std::vector<VkPresentModeKHR> &presentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_MAILBOX_KHR");
            return availablePresentMode;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_IMMEDIATE_KHR");
            bestMode = availablePresentMode;
        }
    }
    EG_CORE_INFO("Swapchain present mode: VK_PRESENT_MODE_FIFO_KHR");
    return  bestMode;
}

VkExtent2D VulkanContext::choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        EG_CORE_INFO_F("Swap extent: width:{0} height:{1}", capabilities.currentExtent.width, capabilities.currentExtent.height);
        return capabilities.currentExtent;
    }
    else {
        VkExtent2D actualExtent = {};
        actualExtent.width = std::clamp<uint32_t>(m_window->get_width(),capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp<uint32_t>(m_window->get_height(),capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        EG_CORE_INFO_F("Swap extent: width:{0} height:{1}", actualExtent.width, actualExtent.height);
        return actualExtent;
    }
}

void VulkanContext::create_swapchain_images() {
    EG_CORE_INFO("Create swapchain images!");

    m_swapchainImageViews.resize(m_swapchainImages.size());

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        m_swapchainImageViews[i] = VulkanHelper::create_image_view(
                m_device,
                m_swapchainImages[i],
                m_swapchainFormat,
                VK_IMAGE_VIEW_TYPE_2D,
                subresourceRange
        );
    }

    EG_CORE_INFO("Swapchain images created!");
}

void VulkanContext::create_render_pass() {

    EG_CORE_TRACE("Creating render pass!");
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = m_swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CALL_ASSERT(vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass)) {
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

    m_framebuffers.resize(m_swapchainImages.size());

    EG_CORE_INFO_F("Number of framebuffers: {0}", m_framebuffers.size());

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
        VkImageView attachments[] = {
                m_swapchainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapchainExtent.width;
        framebufferInfo.height = m_swapchainExtent.height;
        framebufferInfo.layers = 1;

        VK_CALL_ASSERT(vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i])) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
    EG_CORE_TRACE("Framebuffers created!");
}

void VulkanContext::allocate_command_buffers() {

    EG_CORE_TRACE("Allocating primary command buffers!");

    m_commandBuffers.resize(m_swapchainImages.size());
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
        VK_CALL if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                    vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                    vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {

            throw std::runtime_error("failed to create sync objects for a frame!");
        }
    }

    EG_CORE_TRACE("Sync objects created!");
}


void VulkanContext::recreate_swapchain() {
    EG_CORE_TRACE("Recreating swapchain!");

    VK_CALL vkDeviceWaitIdle(m_device);

    //Waits until window is visible
    while(m_window->is_minimized()){
        m_window->wait_native_events();
    }

    cleanup_swapchain();

    create_swapchain();
    create_swapchain_images();
    create_render_pass();
    create_framebuffers();
    allocate_command_buffers();

    VulkanShader::VulkanShaderCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.extent = m_swapchainExtent;
    createInfo.renderPass = m_renderPass;

    for (auto& shader : m_shaders){
        shader->set_vulkan_info(createInfo);
        shader->create_pipeline();
    }

    for (auto& uniformBuffer : m_uniformBuffers){
        uniformBuffer->create_uniform_buffer();
    }

    for (auto& descriptorSet : m_descriptorSets){
        descriptorSet->create_descriptor_pool();
        descriptorSet->create_descriptor_sets();
    }

    EG_CORE_TRACE("Swapchain recreated!");
}

void VulkanContext::cleanup_swapchain() {
    EG_CORE_TRACE("Clearing swapchain!");

    for (size_t i = 0; i < m_framebuffers.size(); i++){
        VK_CALL vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
    }

    VK_CALL vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

    for (auto& descriptorSet : m_descriptorSets){
        descriptorSet->cleanup();
    }

    for (auto& uniformBuffer : m_uniformBuffers){
        uniformBuffer->cleanup();
    }

    for (auto& shader : m_shaders){
        shader->cleanup_pipeline();
    }

    VK_CALL vkDestroyRenderPass(m_device, m_renderPass, nullptr);

    for (size_t i = 0; i < m_swapchainImageViews.size(); i++){
        VK_CALL vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
    }

    VK_CALL vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    EG_CORE_TRACE("Swapchain cleared!");
}

std::weak_ptr<Shader>
VulkanContext::handle_create_shader(const std::string &vertFilePath, const std::string &fragFilePath) {
    EG_CORE_TRACE("Creating a vulkan shader!");
    VulkanShader::VulkanShaderCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.extent = m_swapchainExtent;
    createInfo.renderPass = m_renderPass;
    m_shaders.emplace_back(std::make_shared<VulkanShader>(vertFilePath, fragFilePath, createInfo));
    return m_shaders.back();
}

std::weak_ptr<VertexBuffer>
VulkanContext::handle_create_vertex_buffer(std::vector<float> &vertices, size_t stride) {
    EG_CORE_TRACE("Creating a vulkan vertex buffer!");
    VulkanVertexBufferCreateInfo createInfo = {};
    createInfo.vertices = std::move(vertices);
    createInfo.stride = stride;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_commandPool;
    createInfo.graphicsQueue = m_graphicsQueue;
    m_vertexBuffers.emplace_back(std::make_shared<VulkanVertexBuffer>(m_device, createInfo));
    return m_vertexBuffers.back();
}

std::weak_ptr<IndexBuffer>
VulkanContext::handle_create_index_buffer(std::vector<uint32_t> &indices) {
    EG_CORE_TRACE("Creating a vulkan index buffer!");
    VulkanIndexBufferCreateInfo createInfo = {};
    createInfo.graphicsQueue = m_graphicsQueue;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.commandPool = m_commandPool;
    createInfo.indices = std::move(indices);
    m_indexBuffers.emplace_back(std::make_shared<VulkanIndexBuffer>(m_device, createInfo));
    return m_indexBuffers.back();
}

std::weak_ptr<UniformBuffer>
VulkanContext::handle_create_uniform_buffer(const ShaderItemLayout &layout) {
    VulkanUniformBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.bufferCount = m_swapchainImages.size();
    m_uniformBuffers.emplace_back(std::make_shared<VulkanUniformBuffer>(createInfo, layout));
    return m_uniformBuffers.back();
}

std::weak_ptr<DescriptorSet>
VulkanContext::handle_create_descriptor_set(std::shared_ptr<Shader> shader,
                                            const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers,
                                            const std::vector<std::shared_ptr<Texture2D>> &textures) {
    VulkanDescriptorSetCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.bufferCount = m_swapchainImages.size();
    std::vector<std::shared_ptr<VulkanUniformBuffer>> vulkanUniformBuffers(uniformBuffers.size());
    for (size_t i = 0; i < uniformBuffers.size(); i++){
        vulkanUniformBuffers[i] = std::static_pointer_cast<VulkanUniformBuffer>(uniformBuffers[i]);
    }

    std::vector<std::shared_ptr<VulkanTexture2D>> vulkanTextures(textures.size());
    for (size_t i = 0; i < textures.size(); i++){
        vulkanTextures[i] = std::static_pointer_cast<VulkanTexture2D>(textures[i]);
    }
    m_descriptorSets.emplace_back(std::make_shared<VulkanDescriptorSet>(std::static_pointer_cast<VulkanShader>(shader), vulkanUniformBuffers, vulkanTextures, createInfo));
    return m_descriptorSets.back();
}

std::weak_ptr<Texture2D>
VulkanContext::handle_create_texture_2d(const std::string &filePath) {

    Texture2DCreateInfo textureInfo = Texture2D::load_texture(filePath);

    VulkanTexture2DCreateInfo vulkanTextureCreateInfo = {};
    vulkanTextureCreateInfo.device = m_device;
    vulkanTextureCreateInfo.physicalDevice = m_physicalDevice;
    vulkanTextureCreateInfo.commandPool = m_commandPool;
    vulkanTextureCreateInfo.graphicsQueue = m_graphicsQueue;

    m_textures.emplace_back(std::make_shared<VulkanTexture2D>(textureInfo, vulkanTextureCreateInfo));
    return m_textures.back();
}

void
VulkanContext::handle_draw_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer) {

    if (!m_drawInitialized){
        EG_CORE_ERROR("Draw vertex buffer called outside of draw range!");
        return;
    }

    std::shared_ptr<VulkanVertexBuffer> vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);

    VkBuffer buffers[] = {vulkanVertexBuffer->get_buffer().get_native_buffer()};
    VkDeviceSize offsets[] = {0};

    VK_CALL vkCmdBindVertexBuffers(m_commandBuffers[m_drawInfo.imageIndex], 0, 1, buffers, offsets);
    VK_CALL vkCmdDraw(m_commandBuffers[m_drawInfo.imageIndex], vulkanVertexBuffer->get_vertices_count(), 1, 0, 0);
}

void
VulkanContext::handle_bind_shader(std::shared_ptr<Shader> shader) {

    if (!m_drawInitialized){
        EG_CORE_ERROR("Bind shader called outside of draw range!");
        return;
    }

    std::shared_ptr<VulkanShader> vulkanShader = std::static_pointer_cast<VulkanShader>(shader);

    vulkanShader->bind_command_buffer(m_commandBuffers[m_drawInfo.imageIndex]);
    vulkanShader->bind();
}

void
VulkanContext::handle_draw_indexed_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer,
                                                 std::shared_ptr<IndexBuffer> indexBuffer) {
    if (!m_drawInitialized){
        EG_CORE_ERROR("Draw indexed vertex buffer called outside of draw range!");
        return;
    }
    std::shared_ptr<VulkanVertexBuffer> vulkanVertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(vertexBuffer);

    std::shared_ptr<VulkanIndexBuffer> vulkanIndexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(indexBuffer);

    VkBuffer vertexBuffers[] = {vulkanVertexBuffer->get_buffer().get_native_buffer()};
    VkDeviceSize offsets[] = {0};

    VK_CALL vkCmdBindVertexBuffers(m_commandBuffers[m_drawInfo.imageIndex], 0, 1, vertexBuffers, offsets);

    VK_CALL vkCmdBindIndexBuffer(m_commandBuffers[m_drawInfo.imageIndex], vulkanIndexBuffer->get_buffer().get_native_buffer(), 0, VK_INDEX_TYPE_UINT32);

    VK_CALL vkCmdDrawIndexed(m_commandBuffers[m_drawInfo.imageIndex], vulkanIndexBuffer->get_indices_count(), 1, 0, 0, 0);

}

void
VulkanContext::handle_bind_descriptor_set(std::shared_ptr<DescriptorSet> descriptorSet) {

    if (!m_drawInitialized){
        EG_CORE_ERROR("Draw vertex buffer called outside of draw range!");
        return;
    }

    std::shared_ptr<VulkanDescriptorSet> vulkanDescriptorSet = std::static_pointer_cast<VulkanDescriptorSet>(descriptorSet);

    VulkanDescriptorSetDrawInfo info = {};
    info.bufferIndex = m_drawInfo.imageIndex;
    info.commandBuffer = m_commandBuffers[m_drawInfo.imageIndex];
    vulkanDescriptorSet->set_draw_info(info);
    vulkanDescriptorSet->bind();
}

void VulkanContext::handle_uniform_buffer_update_data(std::shared_ptr<UniformBuffer> uniformBuffer, void *data) {

    std::shared_ptr<VulkanUniformBuffer> vulkanUniformBuffer = std::static_pointer_cast<VulkanUniformBuffer>(uniformBuffer);
    vulkanUniformBuffer->upload_data(data);
    if (std::find(m_dirtyUniformBuffers.begin(), m_dirtyUniformBuffers.end(), vulkanUniformBuffer) == m_dirtyUniformBuffers.end()){
        m_dirtyUniformBuffers.emplace_back(vulkanUniformBuffer);
    }
}

void VulkanContext::handle_window_resized(int width, int height) {
    m_windowResized = true;
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
