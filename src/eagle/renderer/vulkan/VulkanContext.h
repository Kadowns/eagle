//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H


#include <vector>
#include <memory>
#include <functional>

#include "eagle/core/events/WindowEvents.h"
#include "eagle/renderer/RenderingContext.h"

#include "VulkanCore.h"
#include "VulkanShader.h"

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


public:

    VulkanContext();

    virtual ~VulkanContext();

    //inherited via RenderingContext
    virtual void init(Window *window) override;
    virtual void refresh() override;
    virtual void deinit() override;
    //------

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

    virtual void create_swapchain();

    virtual void create_swapchain_images();

    virtual void create_render_pass();

    virtual void create_command_pool();

    virtual void create_framebuffers();

    virtual void allocate_command_buffers();

    virtual void create_sync_objects();

    virtual void recreate_swapchain();

    virtual void cleanup_swapchain();

    bool validation_layers_supported();

    std::vector<const char *> get_required_extensions();

    int evaluate_device(VkPhysicalDevice device);

    QueueFamilyIndices find_family_indices(VkPhysicalDevice device);

    bool check_device_extension_support(VkPhysicalDevice device);

    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats);

    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

    void record_command_buffer(uint32_t index);

    void handle_event(Event& e);

    bool window_resized(WindowResizedEvent& e);

    //inherited via RenderingContext
    virtual std::shared_ptr<Shader>
    handle_create_shader(const std::string &vertFilePath, const std::string &fragFilePath) override;


    Window* m_window;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkQueue m_graphicsQueue;
    VkFormat m_swapchainFormat;
    VkExtent2D m_swapchainExtent;
    VkSwapchainKHR m_swapchain;
    VkRenderPass m_renderPass;
    std::vector<VkImage> m_swapchainImages;
    std::vector<VkImageView> m_swapchainImageViews;
    std::vector<VkFramebuffer> m_framebuffers;

    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    VkQueue m_presentQueue;


    std::vector<std::shared_ptr<VulkanShader>> m_shaders;

    uint32_t m_currentFrame = 0;

    size_t m_eventListenerIdentifier;

    bool m_windowResized = false;


    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

};

_EAGLE_END

#endif //EAGLE_VULKANCONTEXT_H
