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
#include "VulkanTexture2D.h"
#include "VulkanRenderTarget.h"
#include "VulkanCommand.h"
#include "VulkanCommandList.h"

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
    virtual bool begin_draw_commands() override;
    virtual void end_draw_commands() override;
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

    virtual void create_command_pool();

    virtual void allocate_command_buffers();

    virtual void create_sync_objects();

    virtual void create_depth_resources();

    virtual void create_render_pass();

    virtual void create_offscreen_render_pass();

    virtual void create_framebuffers();

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
    VkFormat find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat find_depth_format();

    //inherited via RenderingContext
    virtual Handle<Shader>
    create_shader(const std::string &vertFilePath, const std::string &fragFilePath,
                  const std::vector<Reference<DescriptorSetLayout>> &descriptorSetLayouts,
                  const ShaderPipelineInfo &pipelineInfo) override;

//    virtual Handle<Shader>
//    create_shader(const std::string &vertFilePath, const std::string &fragFilePath,
//                  const std::vector<Reference<DescriptorSetLayout>> &descriptorSetLayouts,
//                  const ShaderPipelineInfo &pipelineInfo,
//                  Reference<RenderTarget> renderTarget) override;

    virtual Handle<VertexBuffer>
    create_vertex_buffer(void *vertices, uint32_t count, const VertexLayout &vertexLayout,
                         EG_BUFFER_USAGE usageFlags) override;

    virtual Handle<IndexBuffer>
    create_index_buffer(void *indexData, size_t indexCount, INDEX_BUFFER_TYPE indexType,
                        EG_BUFFER_USAGE usage) override;

    virtual Handle<UniformBuffer>
    create_uniform_buffer(size_t size, void *data) override;

    virtual Handle<DescriptorSetLayout>
    create_descriptor_set_layout(const std::vector<DescriptorBinding> &bindings) override;

    virtual Handle<DescriptorSet>
    create_descriptor_set(const Reference<DescriptorSetLayout> &descriptorLayout,
                          const std::vector<Reference<DescriptorItem>> &descriptorItems) override;

    virtual Handle<Texture2D>
    create_texture_2d(Texture2DCreateInfo &createInfo) override;

    virtual Handle<RenderTarget>
    create_render_target(const std::vector<RENDER_TARGET_ATTACHMENT> &attachments) override;

    virtual void
    bind_shader(const Reference<Shader> &shader) override;

    virtual void
    draw(uint32_t vertexCount) override;

    virtual void
    draw_indexed(uint32_t indicesCount, uint32_t indexOffset, uint32_t vertexOffset) override;

    virtual void
    uniform_buffer_flush(const Reference<UniformBuffer> &uniformBuffer, void *data) override;

    virtual void
    vertex_buffer_flush(const Reference<VertexBuffer> &vertexBuffer, void *data, uint32_t vertexCount) override;

    virtual void
    index_buffer_flush(const Reference<IndexBuffer> &indexBuffer, void *data, uint32_t indexCount) override;

    virtual void
    bind_descriptor_sets(const Reference<Shader> &shader, const Reference<DescriptorSet> &descriptorSet,
                         uint32_t setIndex) override;

    virtual void
    push_constants(const Reference<Shader> &shader, EG_SHADER_STAGE stage, uint32_t offset, size_t size,
                   void *data) override;

    virtual void
    bind_vertex_buffer(const Reference<VertexBuffer> &vertexBuffer) override;

    virtual void
    bind_index_buffer(const Reference<IndexBuffer> &indexBuffer) override;

    virtual void
    set_viewport(float w, float h, float x, float y, float minDepth, float maxDepth) override;

    virtual void
    set_scissor(uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;

    virtual void begin_draw_offscreen(const Reference<RenderTarget> &renderTarget) override;
    virtual void end_draw_offscreen() override;


protected:

    Window* m_window;

    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkQueue m_graphicsQueue;

    struct {
        VulkanImageAttachment depth;
        VkFormat swapchainFormat;
        VkExtent2D extent2D;
        VkSwapchainKHR swapchain;
        VkRenderPass renderPass, offscreenPass;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> framebuffers;
        VkCommandBuffer commandBuffer;
    } m_present;

    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    VkQueue m_presentQueue;
    VulkanCommandList m_commandList;
    VulkanDrawFrameInfo m_drawInfo;

    std::vector<Reference<VulkanVertexBuffer>> m_vertexBuffers, m_dirtyVertexBuffers;
    std::vector<Reference<VulkanIndexBuffer>> m_indexBuffers, m_dirtyIndexBuffers;
    std::vector<Reference<VulkanUniformBuffer>> m_uniformBuffers, m_dirtyUniformBuffers;
    std::vector<Reference<VulkanDescriptorSet>> m_descriptorSets;
    std::vector<Reference<VulkanDescriptorSetLayout>> m_descriptorSetsLayouts;
    std::vector<Reference<VulkanShader>> m_shaders;
    std::vector<Reference<VulkanTexture2D>> m_textures;
    std::vector<Reference<VulkanRenderTarget>> m_renderTargets;

    uint32_t m_currentFrame = 0;

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

EG_END

#endif //EAGLE_VULKANCONTEXT_H
