//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_VULKANTEXTURE_H
#define EAGLE_VULKANTEXTURE_H

#include "eagle/core/renderer/Texture.h"
#include "VulkanCore.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"

EG_BEGIN

struct VulkanTextureCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};

class VulkanTexture : public Texture {
public:
    VulkanTexture(const TextureCreateInfo &textureCreateInfo,
                  const VulkanTextureCreateInfo &nativeCreateInfo);
    virtual ~VulkanTexture();


    virtual void resize(uint32_t width, uint32_t height) override;

    virtual Reference<Image> image() const override { return m_image; }
    inline const Reference<VulkanImage>& native_image() const { return m_image; }
    inline VkSampler sampler() const { return m_sampler; }

private:
    void create();
    void clear();

private:
    VulkanTextureCreateInfo m_nativeCreateInfo;
    Reference<VulkanImage> m_image;
    VkSampler m_sampler;
};

EG_END

#endif //EAGLE_VULKANTEXTURE_H
