//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_VULKAN_IMAGE_H
#define EAGLE_VULKAN_IMAGE_H

#include "eagle/renderer/Image.h"

#include "VulkanCore.h"
#include <vector>

_EAGLE_BEGIN

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

    virtual std::weak_ptr<ImageAttachment> get_attachment() override {
        return m_attachment;
    }

    virtual std::weak_ptr<ImageSampler> get_sampler() override {
        return m_sampler;
    }

private:
    std::shared_ptr<VulkanImageAttachment> m_attachment;
    std::shared_ptr<VulkanImageSampler> m_sampler;
};


_EAGLE_END

#endif //EAGLE_VULKAN_IMAGE_H
