//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_VULKANTEXTURE2D_H
#define EAGLE_VULKANTEXTURE2D_H

#include "eagle/renderer/Texture2D.h"
#include "VulkanCore.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"

_EAGLE_BEGIN

struct VulkanTexture2DCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanTexture2D : public Texture2D {
public:
    VulkanTexture2D(const Texture2DCreateInfo &textureCreateInfo,
                    const VulkanTexture2DCreateInfo &vulkanInfo);
    ~VulkanTexture2D();

    virtual void upload_pixel_data() override;

    virtual std::weak_ptr<Image> get_image() override;


private:

    void copy_buffer_to_image(std::shared_ptr<VulkanBuffer> buffer, VkImage image, uint32_t width, uint32_t height);

private:
    VulkanTexture2DCreateInfo m_vulkanInfo;
    std::shared_ptr<VulkanImage> m_image;
};

_EAGLE_END

#endif //EAGLE_VULKANTEXTURE2D_H
