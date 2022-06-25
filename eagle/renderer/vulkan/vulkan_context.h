//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H

#include <optional>
#include <functional>

#include "eagle/renderer/rendering_context.h"
#include "eagle/events/window_events.h"

#include "vulkan_global_definitions.h"
#include "vulkan_shader.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_index_buffer.h"
#include "vulkan_uniform_buffer.h"
#include "vulkan_descriptor_set.h"
#include "vulkan_descriptor_set_layout.h"
#include "vulkan_texture.h"
#include "vulkan_command_buffer.h"
#include "vulkan_compute_shader.h"
#include "vulkan_storage_buffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_framebuffer.h"

namespace eagle {

extern VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pCallback);

extern void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT callback,
                                          const VkAllocationCallbacks *pAllocator);

class VulkanContext : public RenderingContext {

protected:

    struct QueueFamilyIndices {//TODO

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && computeFamily.has_value();
        }
    };

    struct SwapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

public:

    VulkanContext();

    ~VulkanContext() override;

    //inherited via RenderingContext
    virtual void destroy();

    bool prepare_frame() override;
    void submit_command_buffer(const std::shared_ptr<CommandBuffer>& commandBuffer) override;
    void present_frame() override;

    std::shared_ptr<RenderPass> main_render_pass() override;
    std::shared_ptr<Framebuffer> main_frambuffer() override;

    const Properties& properties() override;
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

protected:
    virtual void create_logical_device();

    virtual void create_surface() = 0;//implemented per platform

    virtual void create_swapchain();

    virtual void create_command_pool();

    virtual void create_sync_objects();

    virtual void create_render_pass();

    virtual void create_framebuffers();

    virtual void recreate_swapchain();

    virtual void cleanup_swapchain();

    virtual void recreate_objects();

    virtual void clear_objects();

    bool validation_layers_supported();


protected:

    std::vector<const char*> get_extensions();

    virtual std::vector<const char*> get_platform_extensions() = 0;

    int evaluate_device(VkPhysicalDevice device);

    QueueFamilyIndices find_family_indices(VkPhysicalDevice device);

    bool check_device_extension_support(VkPhysicalDevice device);

    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats);

    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

public:
    //inherited via RenderingContext
    std::shared_ptr<Shader>
    create_shader(const ShaderCreateInfo &pipelineInfo) override;

    std::shared_ptr<VertexBuffer>
    create_vertex_buffer(const VertexBufferCreateInfo& usage) override;

    std::shared_ptr<IndexBuffer>
    create_index_buffer(const IndexBufferCreateInfo& createInfo) override;

    std::shared_ptr<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) override;

    std::shared_ptr<DescriptorSetLayout>
    create_descriptor_set_layout(const DescriptorSetLayoutInfo& descriptorSetLayoutInfo) override;

    std::shared_ptr<DescriptorSet>
    create_descriptor_set(const DescriptorSetInfo& descriptorSetInfo) override;

    std::shared_ptr<Texture>
    create_texture(const TextureCreateInfo &createInfo) override;

    std::shared_ptr <RenderPass>
    create_render_pass(RenderPassCreateInfo createInfo) override;

    std::shared_ptr<Framebuffer>
    create_framebuffer(const FramebufferCreateInfo& createInfo) override;

    std::shared_ptr<Image>
    create_image(const ImageCreateInfo& createInfo) override;

    std::shared_ptr<StorageBuffer>
    create_storage_buffer(size_t size, void *data, UpdateType usage) override;

    std::shared_ptr<ComputeShader>
    create_compute_shader(const std::string& path) override;

    std::shared_ptr<CommandBuffer>
    create_command_buffer(const CommandBufferCreateInfo& createInfo) override;

    void destroy_texture_2d(const std::shared_ptr<Texture> &texture) override;

protected:

    Properties m_properties;

    Window* m_window;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    std::shared_ptr<VulkanDeleter> m_deleter;

    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkQueue m_graphicsQueue;

    struct {
        uint32_t imageIndex = 0;
        uint32_t imageCount;
        VkExtent2D extent2D;
        VkSwapchainKHR swapchain;
        std::shared_ptr<VulkanRenderPass> renderPass;
        std::shared_ptr<VulkanFramebuffer> framebuffer;
        std::vector<VkCommandBuffer> commandBuffers;
    } m_present;

    VkCommandPool m_graphicsCommandPool, m_computeCommandPool;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    VkQueue m_presentQueue;


    //compute
    VkQueue m_computeQueue;

    uint32_t m_currentFrame = 0;

    const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

    bool m_windowResized = false;

};

}

#endif //EAGLE_VULKANCONTEXT_H
