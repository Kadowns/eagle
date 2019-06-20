//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_VULKANTEXTURE2D_H
#define EAGLE_VULKANTEXTURE2D_H

#include "eagle/renderer/Texture2D.h"
#include "VulkanCore.h"
#include "VulkanBuffer.h"

_EAGLE_BEGIN

struct VulkanTexture2DCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanTexture2D : public Texture2D{
public:
    VulkanTexture2D(const Texture2DCreateInfo &textureCreateInfo,
                    const VulkanTexture2DCreateInfo &vulkanInfo);
    ~VulkanTexture2D();

    virtual void upload_pixel_data() override;

    inline VkImage get_native_image()          { return m_image;    }
    inline VkDeviceMemory get_native_memory()  { return m_memory;   }
    inline VkImageView get_native_image_view() { return m_imageView;}
    inline VkSampler get_native_sampler()      { return m_sampler;  }

private:

    void copy_buffer_to_image(std::shared_ptr<VulkanBuffer> buffer, VkImage image, uint32_t width, uint32_t height);
    void create_texture_sampler();

private:
    VulkanTexture2DCreateInfo m_vulkanInfo;
    VkImage m_image;
    VkDeviceMemory m_memory;
    VkImageView m_imageView;
    VkSampler m_sampler;

};

_EAGLE_END

#endif //EAGLE_VULKANTEXTURE2D_H
