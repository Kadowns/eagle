//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_VULKANTEXTURE_H
#define EAGLE_VULKANTEXTURE_H

#include "eagle/renderer/texture.h"
#include "vulkan_global_definitions.h"
#include "vulkan_buffer.h"
#include "vulkan_image.h"

namespace eagle {

struct VulkanTextureCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t imageCount;
};

class VulkanTexture : public Texture {
public:
    VulkanTexture(const TextureCreateInfo &textureCreateInfo,
                  const VulkanTextureCreateInfo &nativeCreateInfo);
    virtual ~VulkanTexture();


    virtual void resize(uint32_t width, uint32_t height) override;

    virtual DescriptorType type() const override;
    virtual std::shared_ptr<Image> image() const override { return m_image; }
    inline const std::shared_ptr<VulkanImage>& native_image() const { return m_image; }
    inline VkSampler sampler() const { return m_sampler; }

private:
    void create();
    void clear();

private:
    VulkanTextureCreateInfo m_nativeCreateInfo;
    std::shared_ptr<VulkanImage> m_image;
    VkSampler m_sampler;
};

}

#endif //EAGLE_VULKANTEXTURE_H
