//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H


#include <vector>
#include <optional>
#include <functional>

#include "eagle/core/renderer/RenderingContext.h"

#include "VulkanCore.h"
#include "VulkanShader.h"
#include "VulkanVertexBuffer.h"
#include "VulkanIndexBuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanTexture.h"
//#include "VulkanRenderTarget.h"
#include "VulkanCommand.h"
#include "VulkanCommandList.h"
#include "VulkanCommandBuffer.h"
#include "VulkanComputeShader.h"
#include "VulkanStorageBuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

EG_BEGIN

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
        std::optional<uint32_t> computeFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
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
    virtual void deinit() override;
    virtual void handle_window_resized(int width, int height) override;
    virtual bool prepare_frame() override;
    virtual Reference <Eagle::CommandBuffer> create_command_buffer() override;

    virtual Reference<RenderPass> main_render_pass() override;
    virtual Reference<Framebuffer> main_frambuffer() override;

    virtual void set_recreation_callback(std::function<void()> recreation_callback) override;

    virtual void present_frame() override;
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

    virtual void create_command_pool();

    virtual void allocate_command_buffers();

    virtual void create_sync_objects();

    virtual void create_render_pass();

    virtual void create_framebuffers();

    virtual void create_offscreen_render_pass();

    virtual void recreate_swapchain();

    virtual void cleanup_swapchain();

    bool validation_layers_supported();


protected:

    std::vector<const char *> get_required_extensions();

    int evaluate_device(VkPhysicalDevice device);

    QueueFamilyIndices find_family_indices(VkPhysicalDevice device);

    bool check_device_extension_support(VkPhysicalDevice device);

    SwapChainSupportDetails query_swapchain_support(VkPhysicalDevice device);

    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &formats);

    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& presentModes);

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

public:
    //inherited via RenderingContext
    virtual Handle <Shader>
    create_shader(const std::unordered_map<ShaderStage, std::string> &shaderPaths,
                  const ShaderPipelineInfo &pipelineInfo) override;

    virtual Handle<VertexBuffer>
    create_vertex_buffer(void *vertices, uint32_t count, const VertexLayout &vertexLayout,
                         BufferUsage usageFlags) override;

    virtual Handle<IndexBuffer>
    create_index_buffer(void *indexData, size_t indexCount, IndexBufferType indexType,
                        BufferUsage usage) override;

    virtual Handle<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) override;

    virtual Handle<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBindingDescription> &bindings) override;

    virtual Handle<DescriptorSet>
    create_descriptor_set(const Reference<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<Reference<DescriptorItem>> &descriptorItems) override;

    virtual Handle<Texture>
    create_texture(const TextureCreateInfo &createInfo) override;

    virtual Handle<RenderPass>
    create_render_pass(const std::vector<RenderAttachmentDescription>& colorAttachments, const RenderAttachmentDescription& depthAttachment) override;

    virtual Handle<Framebuffer>
    create_framebuffer(const FramebufferCreateInfo& createInfo) override;

    virtual Handle<Image>
    create_image(const ImageCreateInfo& createInfo) override;

    virtual Handle <StorageBuffer> create_storage_buffer(size_t size, void *data, BufferUsage usage) override;

    virtual Handle<ComputeShader>
    create_compute_shader(const std::string& path) override;

    virtual void destroy_texture_2d(const Reference<Texture> &texture) override;

private:

    void submit_command_buffer(VkCommandBuffer& commandBuffer);


protected:

    Window* m_window;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkQueue m_graphicsQueue;

    struct {
        uint32_t imageIndex = 0;
        uint32_t imageCount;
        VkFormat swapchainFormat;
        VkExtent2D extent2D;
        VkSwapchainKHR swapchain;
        Reference<VulkanRenderPass> renderPass;
        std::vector<Reference<VulkanFramebuffer>> framebuffers;
    } m_present;

    VkCommandPool m_graphicsCommandPool, m_computeCommandPool;
    std::vector<Reference<VulkanCommandBuffer>> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    VkQueue m_presentQueue;


    //compute
    VkQueue m_computeQueue;

    std::vector<Reference<VulkanVertexBuffer>> m_vertexBuffers;
    std::vector<Reference<VulkanIndexBuffer>> m_indexBuffers;
    std::vector<Reference<VulkanUniformBuffer>> m_uniformBuffers;
    std::vector<Reference<VulkanStorageBuffer>> m_storageBuffers;
    std::vector<Reference<VulkanDescriptorSet>> m_descriptorSets;
    std::vector<Reference<VulkanDescriptorSetLayout>> m_descriptorSetsLayouts;
    std::vector<Reference<VulkanShader>> m_shaders;
    std::vector<Reference<VulkanComputeShader>> m_computeShaders;
    std::vector<Reference<VulkanTexture>> m_textures;
    std::vector<Reference<VulkanImage>> m_images;
    std::vector<Reference<VulkanRenderPass>> m_renderPasses;
    std::vector<Reference<VulkanFramebuffer>> m_framebuffers;

    uint32_t m_currentFrame = 0;

    bool m_windowResized = false;
    std::function<void()> recreation_callback;

    const std::vector<const char *> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

    const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const int MAX_FRAMES_IN_FLIGHT = 2;

};

EG_END

#endif //EAGLE_VULKANCONTEXT_H
