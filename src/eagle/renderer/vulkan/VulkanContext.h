//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H


#include <vector>
#include <functional>

#include <vulkan/vulkan.h>

#include "eagle/renderer/RenderingContext.h"

#define VK_CALL m_callInfo.fileName = FILE_BASENAME; m_callInfo.funcName = __FUNCTION__; m_callInfo.line = __LINE__;

#define VK_CALL_IF(result) VK_CALL if (result != VK_SUCCESS)

#define TRACE_ARGS FILE_BASENAME, __FUNCTION__, __LINE__

#define VK_ASSERT(result, message) if (result != VK_SUCCESS) { EG_ERROR(message); throw std::runtime_error(message);}

struct GLFWwindow;

_EAGLE_BEGIN

extern VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pCallback);

extern void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT callback,
                                          const VkAllocationCallbacks *pAllocator);

class VulkanContext : public RenderingContext {

protected:

    struct QueueFamilyIndices {

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct VkCallInfo {
        std::string fileName;
        std::string funcName;
        int line;
    };


public:

    VulkanContext(uint32_t width, uint32_t height);

    virtual ~VulkanContext();

    virtual void init() override;

    virtual void refresh() override;

    virtual void deinit() override;

    virtual void set_window_handle(void *window) override;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
    );

    static bool enableValidationLayers;

protected:

    virtual void create_instance();

    virtual void create_debug_callback();

    virtual void bind_physical_device();

    virtual void create_logical_device();

    virtual void create_surface();

    bool validation_layers_supported();

    std::vector<const char *> get_required_extensions();

    int evaluate_device(VkPhysicalDevice device);

    QueueFamilyIndices find_family_indices(VkPhysicalDevice device);

    bool check_device_extension_support(VkPhysicalDevice device);

    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

    GLFWwindow *m_windowHandle;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkDebugUtilsMessengerEXT m_debugCallback;
    VkCallInfo m_callInfo;
    VkQueue m_graphicsQueue;


    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

};

_EAGLE_END

#endif //EAGLE_VULKANCONTEXT_H
