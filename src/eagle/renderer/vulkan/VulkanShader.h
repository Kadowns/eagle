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

    void create_pipeline(const VulkanShaderCreateInfo &createInfo);
    void cleanup_pipeline();

    VkPipeline& get_pipeline();
    VkPipelineLayout& get_layout();

private:

    static std::vector<char> load_shader(const std::string& fileName);

    VkShaderModule create_shader_module(const std::vector<char>& code);

    std::vector<char> m_vertShaderCode, m_fragShaderCode;

    VulkanShaderCreateInfo m_createInfo;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    bool m_cleared;

};

_EAGLE_END

#endif //EAGLE_VULKANSHADER_H
