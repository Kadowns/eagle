//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_VULKAN_IMAGE_H
#define EAGLE_VULKAN_IMAGE_H

#include "eagle/core/renderer/Image.h"

#include "VulkanCore.h"
#include <vector>

EG_BEGIN

struct VulkanImageAttachment : public ImageAttachment {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
};

struct VulkanImageSampler : public ImageSampler {
    VkSampler sampler;
};

class VulkanImage : public Image {

public:

    VulkanImage(uint32_t width, uint32_t height) :
        Image(width, height),
        m_attachment(std::make_shared<VulkanImageAttachment>()),
        m_sampler(std::make_shared<VulkanImageSampler>()) {}

    virtual Handle<ImageAttachment> get_attachment() override {
        return m_attachment;
    }

    virtual Handle<ImageSampler> get_sampler() override {
        return m_sampler;
    }

private:
    Reference<VulkanImageAttachment> m_attachment;
    Reference<VulkanImageSampler> m_sampler;
};


EG_END

#endif //EAGLE_VULKAN_IMAGE_H
