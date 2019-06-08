//
// Created by Novak on 02/06/2019.
//

#ifndef EAGLE_VULKANHELPER_H
#define EAGLE_VULKANHELPER_H

#include <vector>

#include "VulkanCore.h"

_EAGLE_BEGIN

class VulkanHelper {

public:

    static VkImageView
    create_image_view(VkDevice device, VkImage image, VkFormat format, VkImageViewType viewType, VkImageSubresourceRange subresourceRange);

    static uint32_t
    find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static VkCommandBuffer
    begin_single_time_commands(VkDevice device, VkCommandPool commandPool);

    static void
    end_single_time_commnds(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);

};

_EAGLE_END

#endif //EAGLE_VULKANHELPER_H
