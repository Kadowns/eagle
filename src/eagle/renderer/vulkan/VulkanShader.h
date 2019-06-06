#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "VulkanCore.h"
#include "eagle/renderer/Shader.h"

_EAGLE_BEGIN

class VulkanShader : public Shader {

public:

    struct VulkanShaderCreateInfo{
        VkDevice device;
        VkExtent2D extent;
        VkRenderPass renderPass;
    };

    VulkanShader(const std::string& vertFileName, const std::string& fragFileName, const VulkanShaderCreateInfo& createInfo);
    ~VulkanShader();

    inline void set_vulkan_info(const VulkanShaderCreateInfo& info){ m_createInfo = info; }

    void bind_command_buffer(VkCommandBuffer cmd);

    virtual void bind() override;
    virtual void compile() override;
    virtual void create_pipeline() override;
    virtual void cleanup_pipeline() override;

    VkPipeline& get_pipeline();
    VkPipelineLayout& get_layout();

private:

    VkShaderModule create_shader_module(const std::vector<char>& code);

    VulkanShaderCreateInfo m_createInfo;

    VkCommandBuffer m_cmd;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;

    bool m_cleared;

};

_EAGLE_END

#endif //EAGLE_VULKANSHADER_H
