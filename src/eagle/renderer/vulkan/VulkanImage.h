//
// Created by Novak on 22/06/2019.
//

#ifndef EAGLE_VULKANIMAGE_H
#define EAGLE_VULKANIMAGE_H

#include "VulkanCore.h"

#include "eagle/renderer/Image.h"

struct VulkanImage : public Image {

    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
};


#endif //EAGLE_VULKANIMAGE_H
