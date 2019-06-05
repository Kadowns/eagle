#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "VulkanCore.h"

#include <vector>

_EAGLE_BEGIN

class VulkanShader {

public:

    struct VulkanShaderCreateInfo{
        VkDevice device;
        VkExtent2D extent;
        VkRenderPass renderPass;
    };

    VulkanShader(const std::string& vertFileName, const std::string& fragFileName, const VulkanShaderCreateInfo& createInfo);
    ~VulkanShader();

    VkPipeline& get_pipeline();
    VkPipelineLayout& get_layout();

private:

    VkShaderModule create_shader_module(const std::vector<char>& code);

    static std::vector<char> load_shader(const std::string& fileName);

    VulkanShaderCreateInfo m_createInfo;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

};

_EAGLE_END

#endif //EAGLE_VULKANSHADER_H
