//
// Created by Ricardo on 23/08/2022.
//

#ifndef EAGLE_VULKAN_RENDER_CONTEXT_H
#define EAGLE_VULKAN_RENDER_CONTEXT_H

#include <eagle/renderer/render_context.h>
#include <eagle/renderer/vulkan/vulkan_deleter.h>
#include <eagle/renderer/vulkan/vulkan_queue.h>

namespace eagle {

struct VulkanRenderContextCreateInfo {
    bool enableValidationLayers;
    std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };
};

class VulkanRenderContext : public RenderContext {
protected:
    struct QueueFamilyIndices {

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> computeFamily;

        bool is_complete() const {
            return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
        }

        std::set<uint32_t> unique_indices() const {
            std::set<uint32_t> uniqueIndices;
            if (graphicsFamily){
                uniqueIndices.insert(graphicsFamily.value());
            }
            if (presentFamily){
                uniqueIndices.insert(presentFamily.value());
            }
            if (computeFamily){
                uniqueIndices.insert(computeFamily.value());
            }
            return uniqueIndices;
        }
    };

    struct SwapchainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct SwapchainProperties {
        VkExtent2D extent;
        VkPresentModeKHR presentMode;
        VkSurfaceFormatKHR surfaceFormat;
        VkFormat depthFormat;
        uint32_t imageCount;
    };

public:

    explicit VulkanRenderContext(const RenderContextCreateInfo& createInfo, const VulkanRenderContextCreateInfo& vkCreateInfo);

    ~VulkanRenderContext() override;

    void init() override;

    void exit() override;

    bool prepare_frame(Semaphore *signalAvailableImage) override;

    void present_frame(std::span<Semaphore*> waitSemaphores) override;

    void submit(const CommandBufferSubmitInfo& submitInfo) override;

    std::shared_ptr<Shader> create_shader(const ShaderCreateInfo &shaderCreateInfo) override;

    std::shared_ptr<VertexBuffer> create_vertex_buffer(const VertexBufferCreateInfo &createInfo, void *data, size_t size) override;

    std::shared_ptr<IndexBuffer> create_index_buffer(const IndexBufferCreateInfo &createInfo, void *data, size_t size) override;

    std::shared_ptr<UniformBuffer> create_uniform_buffer(size_t size, void *data) override;

    std::shared_ptr<StorageBuffer> create_storage_buffer(size_t size, void *data, UpdateType usage) override;

    std::shared_ptr<CommandBuffer> create_command_buffer(const CommandBufferCreateInfo &createInfo) override;

    std::shared_ptr<DescriptorSetLayout> create_descriptor_set_layout(const DescriptorSetLayoutInfo &descriptorSetLayoutInfo) override;

    std::shared_ptr<DescriptorSet> create_descriptor_set(const DescriptorSetCreateInfo &descriptorSetInfo) override;

    std::shared_ptr<Image> create_image(const ImageCreateInfo &createInfo) override;

    std::shared_ptr<Texture> create_texture(const TextureCreateInfo &createInfo) override;

    std::shared_ptr<Framebuffer> create_framebuffer(const FramebufferCreateInfo &createInfo) override;

    std::shared_ptr<RenderPass> create_render_pass(RenderPassCreateInfo createInfo) override;

    std::shared_ptr<Fence> create_fence() override;

    std::shared_ptr<Semaphore> create_semaphore() override;

    Framebuffer* main_framebuffer() override;

    RenderPass* main_render_pass() override;

protected:

    ///helper functions------------------

    //may be overridden by derived classes for platform specific extensions
    virtual std::vector<const char*> query_instance_extensions();

    virtual std::vector<const char*> query_device_extensions();

    virtual QueueFamilyIndices query_queue_family_indices(VkPhysicalDevice device);

    virtual SwapchainSupportDetails query_swapchain_details(VkPhysicalDevice device);

    virtual VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats);

    virtual VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes);

    virtual VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

    virtual void debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData);

    ///main initializing functions-------
    virtual void create_instance();

    virtual void select_physical_device();

    //must be implemented per platform
    virtual void create_surface() = 0;

    virtual void create_logical_device();

    virtual void create_queues();

    virtual void create_swapchain();

    virtual void create_deleter();

    virtual void create_main_framebuffers();

    virtual void create_main_render_pass();
    //-------------------------------------

protected:

    VulkanRenderContextCreateInfo m_vkCreateInfo;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;

    QueueFamilyIndices m_queueFamilyIndices;
    SwapchainProperties m_swapchainProperties = {};
    uint32_t m_currentSwapchainImage = 0;
    uint32_t m_currentFrame = 0;

    std::unique_ptr<VulkanDeleter> m_deleter;
    std::unordered_map<QueueType, std::unique_ptr<VulkanQueue>> m_queues;
    std::shared_ptr<Framebuffer> m_mainFramebuffer;
    std::shared_ptr<RenderPass> m_mainRenderPass;
    bool m_windowResized = false;
};

} // eagle

#endif //EAGLE_VULKAN_RENDER_CONTEXT_H
