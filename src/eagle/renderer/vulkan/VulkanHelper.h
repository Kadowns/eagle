//
// Created by Novak on 02/06/2019.
//

#ifndef EAGLE_VULKANHELPER_H
#define EAGLE_VULKANHELPER_H

#include "VulkanCore.h"

_EAGLE_BEGIN

class VulkanHelper {

public:




    static VkImageView create_image_view(VkDevice device, VkImage image, VkFormat format, VkImageViewType viewType, VkImageSubresourceRange subresourceRange);


};

_EAGLE_END

#endif //EAGLE_VULKANHELPER_H
