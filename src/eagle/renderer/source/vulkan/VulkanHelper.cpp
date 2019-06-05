//
// Created by Novak on 02/06/2019.
//


#include "eagle/renderer/vulkan/VulkanHelper.h"

_EAGLE_BEGIN

VkImageView VulkanHelper::create_image_view(
        VkDevice device,
        VkImage image,
        VkFormat format,
        VkImageViewType viewType,
        VkImageSubresourceRange subresourceRange) {

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange = subresourceRange;

    VkImageView imageView;
    VK_CALL_ASSERT(vkCreateImageView(device, &viewInfo, nullptr, &imageView)) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

_EAGLE_END


