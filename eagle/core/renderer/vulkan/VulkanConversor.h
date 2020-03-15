//
// Created by Novak on 31/08/2019.
//

#ifndef EAGLE_VULKANCONVERSOR_H
#define EAGLE_VULKANCONVERSOR_H

#include "VulkanCore.h"

EG_BEGIN

class VulkanConversor {
public:
    static VkFormat           to_vk(Format format);
    static Format           to_eg(VkFormat format);
    static VkShaderStageFlags to_vk(ShaderStage stage);
    static ShaderStage      to_eg(VkShaderStageFlags stage);
    static VkDescriptorType   to_vk(DescriptorType type);
    static DescriptorType   to_eg(VkDescriptorType type);
    static VkFilter           to_vk(Filter filter);
    static Filter           to_eg(VkFilter filter);
};

EG_END

#endif //EAGLE_VULKANCONVERSOR_H
