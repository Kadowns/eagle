//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H


#include <vector>

#include <vulkan/vulkan.h>

#include "eagle/renderer/RenderingContext.h"

_EAGLE_BEGIN

class VulkanContext : public RenderingContext {

public:

    VulkanContext(uint32_t width, uint32_t height);

    virtual ~VulkanContext();

    virtual void init()   override;
    virtual void refresh()                              override;
    virtual void deinit()                               override;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
    );

    static bool enableValidationLayers;

protected:

    virtual void create_instance();
    virtual void create_debug_callback();


    bool validation_layers_supported();
    std::vector<const char*> get_required_extensions();

    VkInstance m_instance;
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;

    VkDebugUtilsMessengerEXT m_debugCallback;


    const std::vector<const char*> validationLayers = {
            "VK_LAYER_LUNARG_standard_validation"
    };

};

_EAGLE_END

#endif //EAGLE_VULKANCONTEXT_H
