//
// Created by Novak on 23/03/2020.
//

#ifndef EAGLE_VULKANSHADERUTILS_H
#define EAGLE_VULKANSHADERUTILS_H

#include "VulkanCore.h"
#include <eagle/core/renderer/DescriptorSetLayout.h>

EG_BEGIN

class VulkanShaderUtils {
public:

    static void add_bindings_from_shader_stage(const std::vector<uint32_t> &code, VkShaderStageFlags stage,
                                               std::map<uint32_t, std::map<uint32_t, DescriptorBindingDescription>> &descriptorSetMap,
                                               std::vector<VkPushConstantRange> &pushConstantsRanges);


    static void enumerate_output_variables(const std::vector<uint32_t>& code, uint32_t& outputVariableCount);

    static VkShaderModule create_shader_module(VkDevice device, const std::vector<uint32_t> &code);

};

EG_END

#endif //EAGLE_VULKANSHADERUTILS_H
