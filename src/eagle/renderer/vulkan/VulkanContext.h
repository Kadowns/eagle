//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H


#include <vector>
#include <memory>
#include <optional>
#include <functional>

#include "eagle/core/events/WindowEvents.h"
#include "eagle/renderer/RenderingContext.h"

#include "VulkanCore.h"
#include "VulkanShader.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanTexture2D.h"

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

    struct VulkanDrawFrameInfo {
        bool invalidFrame = false;
        uint32_t imageIndex;
    };


public:

    VulkanContext();

    virtual ~VulkanContext();

    //inherited via RenderingContext
    virtual void init(Window *window) override;
    virtual void begin_draw() override;
    virtual void end_draw() override;
    virtual void refresh() override;
    virtual void deinit() override;
    virtual void handle_window_resized(int width, int height) override;
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

    //inherited via RenderingContext
    virtual std::weak_ptr<Shader>
    handle_create_shader(const std::string &vertFilePath, const std::string &fragFilePath) override;

    virtual std::weak_ptr<VertexBuffer>
    handle_create_vertex_buffer(std::vector<float> &vertices, size_t stride) override;

    virtual std::weak_ptr<IndexBuffer>
    handle_create_index_buffer(std::vector<uint32_t> &indices) override;

    virtual std::weak_ptr<UniformBuffer>
    handle_create_uniform_buffer(const ShaderItemLayout& layout) override;

    virtual std::weak_ptr<DescriptorSet>
    handle_create_descriptor_set(std::shared_ptr<Shader> shader,
                                 const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers,
                                 const std::vector<std::shared_ptr<Texture2D>> &textures) override;

    virtual std::weak_ptr<Texture2D>
    handle_create_texture_2d(const std::string& filePath) override;

    virtual void
    handle_bind_shader(std::shared_ptr<Shader> shader) override;

    virtual void
    handle_draw_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer) override;

    virtual void
    handle_draw_indexed_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) override;

    virtual void
    handle_uniform_buffer_update_data(std::shared_ptr<UniformBuffer> uniformBuffer, void *data) override;

    virtual void
    handle_bind_descriptor_set(std::shared_ptr<DescriptorSet> descriptorSet) override;

protected:

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
    VulkanDrawFrameInfo m_drawInfo;

    std::vector<std::shared_ptr<VulkanVertexBuffer>> m_vertexBuffers;
    std::vector<std::shared_ptr<VulkanIndexBuffer>> m_indexBuffers;
    std::vector<std::shared_ptr<VulkanUniformBuffer>> m_uniformBuffers, m_dirtyUniformBuffers;
    std::vector<std::shared_ptr<VulkanDescriptorSet>> m_descriptorSets;
    std::vector<std::shared_ptr<VulkanShader>> m_shaders;
    std::vector<std::shared_ptr<VulkanTexture2D>> m_textures;

    uint32_t m_currentFrame = 0;

    size_t m_eventListenerIdentifier;

    bool m_windowResized = false;
    bool m_drawInitialized = false;


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
