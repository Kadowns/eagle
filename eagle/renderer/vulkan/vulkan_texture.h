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
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    uint32_t frameCount = 0;
    VulkanCommandQueue* queue = nullptr;
};

class VulkanTexture : public Texture {
public:
    VulkanTexture(const TextureCreateInfo &textureCreateInfo,
                  const VulkanTextureCreateInfo &nativeCreateInfo);
    virtual ~VulkanTexture();


    virtual void resize(uint32_t width, uint32_t height) override;

    virtual DescriptorType type() const override;
    virtual Image* image() const override { return m_image.get(); }
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
