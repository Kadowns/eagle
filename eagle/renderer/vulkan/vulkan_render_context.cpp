//
// Created by Ricardo on 23/08/2022.
//

#include <eagle/renderer/vulkan/vulkan_render_context.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/window.h>

namespace eagle {

namespace detail {

static std::vector<VkSemaphore> native_semaphores(const std::span<Semaphore*> semaphores, size_t frameIndex){
    std::vector<VkSemaphore> nativeSemaphores;
    nativeSemaphores.reserve(semaphores.size());
    for (auto semaphore : semaphores){
        auto castedSemaphore = (VulkanSemaphore*)semaphore;
        nativeSemaphores.emplace_back(castedSemaphore->native_semaphore(frameIndex));
    }
    return nativeSemaphores;
}

static std::vector<VkCommandBuffer> native_command_buffers(const std::span<CommandBuffer*>& commandBuffers, uint32_t frameIndex) {
    std::vector<VkCommandBuffer> result;
    result.reserve(commandBuffers.size());

    for (auto commandBuffer : commandBuffers) {
        auto castedCommandBuffer = (VulkanCommandBuffer*)commandBuffer;
        result.push_back(castedCommandBuffer->native_command_buffer(frameIndex));
    }

    return result;
}

static std::vector<VkPipelineStageFlags> native_pipeline_stages(const std::span<PipelineStageFlags>& pipelineStages){
    std::vector<VkPipelineStageFlags> result;
    result.reserve(pipelineStages.size());

    for (auto stageMask : pipelineStages){
        result.push_back(VulkanConverter::eg_flags_to_vk_flags<PipelineStageFlagsBits>(stageMask));
    }

    return result;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
){
    auto renderContext = (VulkanRenderContext*)pUserData;
    renderContext->debug_callback(messageSeverity, messageType, pCallbackData);
    return VK_FALSE;
}

static VkResult create_debug_utils_messenger(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pCallback) {

    auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkCreateDebugUtilsMessengerEXT != nullptr) {
        return vkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pCallback);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void destroy_debug_utils_messenger(VkInstance instance,
                                   VkDebugUtilsMessengerEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

template<typename T>
std::shared_ptr<T> make_shared_with_deleter(T* ptr, std::unique_ptr<VulkanDeleter>& deleter){
    return std::shared_ptr<T>(ptr, [&deleter](T* ptr){
        deleter->destroy(ptr);
    });
}

}

VulkanRenderContext::VulkanRenderContext(const RenderContextCreateInfo &createInfo, const VulkanRenderContextCreateInfo& vkCreateInfo) :
    RenderContext(createInfo),
    m_vkCreateInfo(vkCreateInfo) {

}

VulkanRenderContext::~VulkanRenderContext() {

}

void VulkanRenderContext::init() {
    create_instance();
    select_physical_device();
    create_surface();
    create_logical_device();
    create_queues();
    create_swapchain();
    create_deleter();
    create_main_render_pass();
    create_main_framebuffers();
}

void VulkanRenderContext::exit() {

    vkDeviceWaitIdle(m_device);

    m_deleter->destroy(m_mainFramebuffer.release());
    m_deleter->destroy(m_mainRenderPass.release());

    m_deleter->clear_all();

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    m_queues.clear();

    vkDestroyDevice(m_device, nullptr);

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    if (m_vkCreateInfo.enableValidationLayers) {
        detail::destroy_debug_utils_messenger(m_instance, m_debugMessenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);

}

bool VulkanRenderContext::prepare_frame(Semaphore *signalAvailableImage) {
    auto castedSemaphore = (VulkanSemaphore*)signalAvailableImage;

    VkResult result;

    result = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            std::numeric_limits<uint64_t>::max(),
            castedSemaphore->native_semaphore(m_currentFrame),
            VK_NULL_HANDLE,
            &m_currentSwapchainImage);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreate_swapchain();
        return false;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw VulkanException("failed to acquire swapchain image!", result);
    }

    //updates dirty buffers-------------------------
    VulkanCleaner::flush(m_currentFrame);
    m_deleter->clear_frame();
    return true;
}

void VulkanRenderContext::present_frame(std::span<Semaphore*> waitSemaphores) {
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    auto semaphores = detail::native_semaphores(waitSemaphores, m_currentFrame);

    presentInfo.waitSemaphoreCount = semaphores.size();
    presentInfo.pWaitSemaphores = semaphores.data();

    VkSwapchainKHR swapChains[] = {m_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_currentSwapchainImage;

    auto it = m_queues.find(CommandQueueType::PRESENT);
    if (it == m_queues.end()){
        throw std::runtime_error("no present queue was found when presenting frame");
    }
    auto& queue = it->second;

    auto result = vkQueuePresentKHR(queue->native_queue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_windowResized) {
        m_windowResized = false;
        recreate_swapchain();
    }
    else if (result != VK_SUCCESS) {
        throw VulkanException("failed to present swapchain image!", result);
    }

    m_currentFrame = (m_currentFrame + 1) % m_createInfo.maxFramesInFlight;
}

std::shared_ptr<Shader> VulkanRenderContext::create_shader(const ShaderCreateInfo &shaderCreateInfo) {
    VulkanShaderCreateInfo nativeCreateInfo = {};
    nativeCreateInfo.device = m_device;
    nativeCreateInfo.extent = m_swapchainProperties.extent;
    nativeCreateInfo.on_window_resized = &on_window_resized;

    auto ptr = new VulkanShader(shaderCreateInfo, nativeCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<VertexBuffer> VulkanRenderContext::create_vertex_buffer(const VertexBufferCreateInfo &createInfo, void *data, size_t size) {
    VulkanVertexBufferCreateInfo nativeCreateInfo = {};
    nativeCreateInfo.physicalDevice = m_physicalDevice;
    nativeCreateInfo.device = m_device;
    nativeCreateInfo.queue = m_queues.at(CommandQueueType::GRAPHICS).get();
    nativeCreateInfo.frameCount = m_createInfo.maxFramesInFlight;

    auto ptr = new VulkanVertexBuffer(createInfo, nativeCreateInfo, data, size);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<IndexBuffer> VulkanRenderContext::create_index_buffer(const IndexBufferCreateInfo &createInfo, void *data, size_t size) {
    VulkanIndexBufferCreateInfo nativeCreateInfo = {};
    nativeCreateInfo.device = m_device;
    nativeCreateInfo.physicalDevice = m_physicalDevice;
    nativeCreateInfo.frameCount = m_createInfo.maxFramesInFlight;
    nativeCreateInfo.queue = m_queues.at(CommandQueueType::GRAPHICS).get();

    auto ptr = new VulkanIndexBuffer(createInfo, nativeCreateInfo, data, size);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<UniformBuffer> VulkanRenderContext::create_uniform_buffer(size_t size, void *data) {
    VulkanUniformBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.frameCount = m_createInfo.maxFramesInFlight;

    auto ptr = new VulkanUniformBuffer(createInfo, size, data);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<StorageBuffer> VulkanRenderContext::create_storage_buffer(size_t size, void *data, UpdateType usage) {
    VulkanStorageBufferCreateInfo createInfo = {};
    createInfo.device = m_device;
    createInfo.physicalDevice = m_physicalDevice;
    createInfo.frameCount = m_createInfo.maxFramesInFlight;
    createInfo.queue = m_queues.at(CommandQueueType::GRAPHICS).get();

    auto ptr = new VulkanStorageBuffer(createInfo, size, data, usage);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<CommandBuffer> VulkanRenderContext::create_command_buffer(const CommandBufferCreateInfo &createInfo) {
    VulkanCommandBufferCreateInfo vkCreateInfo = {};
    vkCreateInfo.device = m_device;
    vkCreateInfo.frameCount = m_createInfo.maxFramesInFlight;
    vkCreateInfo.currentFrame = &m_currentFrame;

    auto ptr = new VulkanCommandBuffer(createInfo, vkCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<DescriptorSetLayout>
VulkanRenderContext::create_descriptor_set_layout(const DescriptorSetLayoutCreateInfo &descriptorSetLayoutInfo) {
    VulkanDescriptorSetLayoutCreateInfo vulkanDescriptorSetLayoutCreateInfo = {};
    vulkanDescriptorSetLayoutCreateInfo.device = m_device;

    auto ptr = new VulkanDescriptorSetLayout(descriptorSetLayoutInfo, vulkanDescriptorSetLayoutCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<DescriptorSet> VulkanRenderContext::create_descriptor_set(const DescriptorSetCreateInfo &descriptorSetInfo) {
    VulkanDescriptorSetInfo vulkanDescriptorSetInfo = {};
    vulkanDescriptorSetInfo.device = m_device;
    vulkanDescriptorSetInfo.frameCount = m_createInfo.maxFramesInFlight;

    auto ptr = new VulkanDescriptorSet(descriptorSetInfo, vulkanDescriptorSetInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<Image> VulkanRenderContext::create_image(const ImageCreateInfo &createInfo) {
    VulkanImageCreateInfo vulkanImageCreateInfo = {};
    vulkanImageCreateInfo.device = m_device;
    vulkanImageCreateInfo.physicalDevice = m_physicalDevice;
    vulkanImageCreateInfo.frameCount = m_createInfo.maxFramesInFlight;

    auto ptr = new VulkanImage(createInfo, vulkanImageCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<Texture> VulkanRenderContext::create_texture(const TextureCreateInfo &createInfo) {
    VulkanTextureCreateInfo vulkanTextureCreateInfo = {};
    vulkanTextureCreateInfo.device = m_device;
    vulkanTextureCreateInfo.physicalDevice = m_physicalDevice;
    vulkanTextureCreateInfo.queue = m_queues.at(CommandQueueType::GRAPHICS).get();
    vulkanTextureCreateInfo.frameCount = m_createInfo.maxFramesInFlight;

    auto ptr = new VulkanTexture(createInfo, vulkanTextureCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<Framebuffer> VulkanRenderContext::create_framebuffer(const FramebufferCreateInfo &createInfo) {
    VulkanFramebufferCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;
    vulkanCreateInfo.frameCount = m_createInfo.maxFramesInFlight;
    vulkanCreateInfo.currentFrame = &m_currentFrame;

    auto ptr = new VulkanFramebuffer(createInfo, vulkanCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<RenderPass> VulkanRenderContext::create_render_pass(RenderPassCreateInfo createInfo) {
    VulkanRenderPassCreateInfo vulkanCreateInfo = {};
    vulkanCreateInfo.device = m_device;

    auto ptr = new VulkanRenderPass(createInfo, vulkanCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<Fence> VulkanRenderContext::create_fence() {
    VulkanFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.frameCount = m_createInfo.maxFramesInFlight;
    fenceCreateInfo.device = m_device;
    fenceCreateInfo.currentFrame = &m_currentFrame;
    auto ptr = new VulkanFence(fenceCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

std::shared_ptr<Semaphore> VulkanRenderContext::create_semaphore() {
    VulkanSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.frameCount = m_createInfo.maxFramesInFlight;
    semaphoreCreateInfo.device = m_device;

    auto ptr = new VulkanSemaphore(semaphoreCreateInfo);
    return detail::make_shared_with_deleter(ptr, m_deleter);
}

Framebuffer* VulkanRenderContext::main_framebuffer() {
    return m_mainFramebuffer.get();
}

RenderPass* VulkanRenderContext::main_render_pass() {
    return m_mainRenderPass.get();
}

CommandQueue* VulkanRenderContext::command_queue(CommandQueueType queueType) {
    auto it = m_queues.find(queueType);
    if (it == m_queues.end()){
        throw std::runtime_error("requested a command queue that does not exist");
    }
    return it->second.get();
}

void VulkanRenderContext::debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {

#define VK_LOG(level, message) spdlog::get("eagle")->log(level, message);

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
}

std::vector<const char*> VulkanRenderContext::query_instance_extensions() {
    std::vector<const char*> extensions = {VK_KHR_SURFACE_EXTENSION_NAME};
    if (m_vkCreateInfo.enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return extensions;
}

std::vector<const char*> VulkanRenderContext::query_device_extensions() {
    return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
}

VulkanRenderContext::QueueFamilyIndices VulkanRenderContext::query_queue_family_indices(VkPhysicalDevice device) {

    if (m_surface == VK_NULL_HANDLE){
        throw std::logic_error("tried to query queue family indices with a null surface");
    }

    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int queueIndex = 0;
    for (const auto &queueFamily : queueFamilies) {

        VkBool32 presentSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, m_surface, &presentSupport);
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = queueIndex;
        }
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = queueIndex;
        }
        if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            indices.computeFamily = queueIndex;
        }

        if (indices.is_complete()) {
            break;
        }
        queueIndex++;
    }

    //No dedicated compute queue found! Searching for any suitable queue family
    if (!indices.computeFamily.has_value()){
        queueIndex = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT){
                indices.computeFamily = queueIndex;
                break;
            }
            queueIndex++;
        }
    }

    return indices;
}

VulkanRenderContext::SwapchainSupportDetails VulkanRenderContext::query_swapchain_details(VkPhysicalDevice device) {
    if (m_surface == VK_NULL_HANDLE) {
        throw std::logic_error("tried to query swapchain details for device without a valid surface");
    }

    SwapchainSupportDetails details = {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());
    }
    return details;
}

VkSurfaceFormatKHR VulkanRenderContext::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
    if (formats.size() == 1 && formats.front().format == VK_FORMAT_UNDEFINED) {
        return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
    }

    for (const auto &availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return formats.front();
}

VkPresentModeKHR VulkanRenderContext::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes) {
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto availablePresentMode : presentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            bestMode = availablePresentMode;
        }
    }
    return bestMode;
}

VkExtent2D VulkanRenderContext::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D actualExtent = {};
    actualExtent.width = std::clamp<uint32_t>(m_createInfo.window->width(),
                                              capabilities.minImageExtent.width,
                                              capabilities.maxImageExtent.width);

    actualExtent.height = std::clamp<uint32_t>(m_createInfo.window->height(),
                                               capabilities.minImageExtent.height,
                                               capabilities.maxImageExtent.height);
    return actualExtent;
}

void VulkanRenderContext::create_instance() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = m_createInfo.applicationName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = m_createInfo.engineName;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = query_instance_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (m_vkCreateInfo.enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_vkCreateInfo.validationLayers.size());
        createInfo.ppEnabledLayerNames = m_vkCreateInfo.validationLayers.data();

        debugCreateInfo = {};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = detail::debug_callback;
        debugCreateInfo.pUserData = this;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    auto result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS){
        throw VulkanException("failed to create VkInstance", result);
    }

    if (!m_vkCreateInfo.enableValidationLayers) {
        return;
    }

    result = detail::create_debug_utils_messenger(m_instance, &debugCreateInfo, nullptr, &m_debugMessenger);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to setup debug callback!", result);
    }
}

void VulkanRenderContext::select_physical_device() {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    m_physicalDevice = devices[m_createInfo.physicalDeviceIndex];
}

void VulkanRenderContext::create_logical_device() {

    m_queueFamilyIndices = query_queue_family_indices(m_physicalDevice);
    auto uniqueQueueFamilies = m_queueFamilyIndices.unique_indices();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
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

    auto deviceExtensions = query_device_extensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    // Note: this is ignored on modern vulkan implementations,
    // but it's good to keep here for compatibility with older versions
    // (you never know when you're going to need it)
    if (m_vkCreateInfo.enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_vkCreateInfo.validationLayers.size());
        createInfo.ppEnabledLayerNames = m_vkCreateInfo.validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    auto result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create logical device", result);
    }
}

void VulkanRenderContext::create_queues() {

    if (m_device == VK_NULL_HANDLE){
        throw std::logic_error("tried to create queues without a valid VkDevice");
    }

    VulkanQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.device = m_device;
    queueCreateInfo.index = 0;
    queueCreateInfo.currentFrame = &m_currentFrame;

    if (m_queueFamilyIndices.presentFamily.has_value()) {
        queueCreateInfo.familyIndex = m_queueFamilyIndices.presentFamily.value();
        queueCreateInfo.type = CommandQueueType::PRESENT;
        m_queues.emplace(CommandQueueType::PRESENT, std::make_unique<VulkanCommandQueue>(queueCreateInfo));
    }

    if (m_queueFamilyIndices.graphicsFamily.has_value()) {
        queueCreateInfo.familyIndex = m_queueFamilyIndices.graphicsFamily.value();
        queueCreateInfo.type = CommandQueueType::GRAPHICS;
        m_queues.emplace(CommandQueueType::GRAPHICS, std::make_unique<VulkanCommandQueue>(queueCreateInfo));
    }

    if (m_queueFamilyIndices.computeFamily.has_value()) {
        queueCreateInfo.familyIndex = m_queueFamilyIndices.computeFamily.value();
        queueCreateInfo.type = CommandQueueType::COMPUTE;
        m_queues.emplace(CommandQueueType::COMPUTE, std::make_unique<VulkanCommandQueue>(queueCreateInfo));
    }
}

void VulkanRenderContext::create_swapchain() {
    auto swapchainDetails = query_swapchain_details(m_physicalDevice);

    m_swapchainProperties.surfaceFormat = choose_swap_surface_format(swapchainDetails.formats);

    m_swapchainProperties.presentMode = choose_swap_present_mode(swapchainDetails.presentModes);

    m_swapchainProperties.extent = choose_swap_extent(swapchainDetails.capabilities);

    //Old code, should check if this is right
    m_swapchainProperties.imageCount = swapchainDetails.capabilities.minImageCount + 1;
    if (swapchainDetails.capabilities.maxImageCount > 0 && m_swapchainProperties.imageCount > swapchainDetails.capabilities.maxImageCount) {
        m_swapchainProperties.imageCount = swapchainDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = m_swapchainProperties.imageCount;
    createInfo.imageFormat = m_swapchainProperties.surfaceFormat.format;
    createInfo.imageColorSpace = m_swapchainProperties.surfaceFormat.colorSpace;
    createInfo.imageExtent = m_swapchainProperties.extent;
    createInfo.imageArrayLayers = 1;
    //We use VK_IMAGE_USAGE_TRANSFER_DST_BIT, so we can support some types of post processing
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    std::set<uint32_t> uniqueFamilies = m_queueFamilyIndices.unique_indices();
    std::vector<uint32_t> queueFamilyIndices(uniqueFamilies.begin(), uniqueFamilies.end());

    if (m_queueFamilyIndices.graphicsFamily != m_queueFamilyIndices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
        createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapchainDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = m_swapchainProperties.presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    auto result = vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain);
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create VkSwapchainKHR", result);
    }
}

void VulkanRenderContext::create_deleter() {

    VulkanDeleterCreateInfo info = {};
    info.frameCount = m_createInfo.maxFramesInFlight;
    info.device = m_device;

    m_deleter = std::make_unique<VulkanDeleter>(info);
}

void VulkanRenderContext::create_main_render_pass() {

    RenderAttachmentDescription colorAttachment = {};
    colorAttachment.format = VulkanConverter::to_eg(m_swapchainProperties.surfaceFormat.format);
    colorAttachment.loadOp = AttachmentLoadOperator::CLEAR;
    colorAttachment.storeOp = AttachmentStoreOperator::STORE;
    colorAttachment.stencilLoadOp = AttachmentLoadOperator::DONT_CARE;
    colorAttachment.stencilStoreOp = AttachmentStoreOperator::DONT_CARE;
    colorAttachment.initialLayout = ImageLayout::UNDEFINED;
    colorAttachment.finalLayout = ImageLayout::PRESENT_SRC_KHR;

    m_swapchainProperties.depthFormat = VulkanHelper::find_depth_format(m_physicalDevice);

    RenderAttachmentDescription depthAttachment = {};
    depthAttachment.format = VulkanConverter::to_eg(m_swapchainProperties.depthFormat);
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

    m_mainRenderPass = std::make_unique<VulkanRenderPass>(std::move(renderPassCreateInfo), vulkanCreateInfo);

}

void VulkanRenderContext::create_main_framebuffers() {

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchainProperties.imageCount, nullptr);

    std::vector<VkImage> swapchainImages(m_swapchainProperties.imageCount);

    vkGetSwapchainImagesKHR(m_device, m_swapchain, &m_swapchainProperties.imageCount, swapchainImages.data());

    //we will create a framebuffer object for each swapchain image separately

    ImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.width = m_swapchainProperties.extent.width;
    imageCreateInfo.height = m_swapchainProperties.extent.height;
    imageCreateInfo.format = VulkanConverter::to_eg(m_swapchainProperties.surfaceFormat.format);
    imageCreateInfo.aspects = {IMAGE_ASPECT_COLOR};
    imageCreateInfo.usages = {IMAGE_USAGE_COLOR_ATTACHMENT};
    imageCreateInfo.owningQueue = m_queues.at(CommandQueueType::GRAPHICS).get();

    VulkanImageCreateInfo nativeImageCreateInfo = {};
    nativeImageCreateInfo.physicalDevice = m_physicalDevice;
    nativeImageCreateInfo.device = m_device;
    nativeImageCreateInfo.frameCount = m_swapchainProperties.imageCount;

    std::shared_ptr<VulkanImage> colorImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo, swapchainImages);

    //depth
    imageCreateInfo.format = VulkanConverter::to_eg(m_swapchainProperties.depthFormat);
    imageCreateInfo.aspects = {IMAGE_ASPECT_DEPTH};
    imageCreateInfo.usages = {IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT};
    imageCreateInfo.layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    imageCreateInfo.tiling = ImageTiling::OPTIMAL;
    imageCreateInfo.memoryProperties = {MEMORY_PROPERTY_DEVICE_LOCAL};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    if (imageCreateInfo.format == Format::D32_SFLOAT_S8_UINT || imageCreateInfo.format == Format::D24_UNORM_S8_UINT) {
        imageCreateInfo.aspects |= IMAGE_ASPECT_STENCIL;
    }

    std::shared_ptr<VulkanImage> depthImage = std::make_shared<VulkanImage>(imageCreateInfo, nativeImageCreateInfo);

    FramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.width = m_swapchainProperties.extent.width;
    framebufferCreateInfo.height = m_swapchainProperties.extent.height;
    framebufferCreateInfo.renderPass = m_mainRenderPass.get();
    framebufferCreateInfo.attachments = {colorImage, depthImage};

    VulkanFramebufferCreateInfo nativeFramebufferCreateInfo = {};
    nativeFramebufferCreateInfo.device = m_device;
    nativeFramebufferCreateInfo.frameCount = m_swapchainProperties.imageCount;
    nativeFramebufferCreateInfo.currentFrame = &m_currentSwapchainImage;

    m_mainFramebuffer = std::make_unique<VulkanFramebuffer>(framebufferCreateInfo, nativeFramebufferCreateInfo);

    EG_TRACE("eagle", "Framebuffers created!");
}

void VulkanRenderContext::recreate_swapchain() {

    vkDeviceWaitIdle(m_device);

    //Waits until window is visible
    while (m_createInfo.window->is_minimized()) {
        m_createInfo.window->wait_native_events();
    }

    m_deleter->destroy(m_mainFramebuffer.release());
    m_deleter->clear_all();
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    create_swapchain();
    create_main_framebuffers();

    on_window_resized(m_swapchainProperties.extent);
}

} // eagle