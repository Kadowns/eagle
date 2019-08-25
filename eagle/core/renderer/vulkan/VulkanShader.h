#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "eagle/core/renderer/Shader.h"
#include "VulkanCore.h"
#include "VulkanRenderTarget.h"
#include "VulkanDescriptorSetLayout.h"

EG_BEGIN

class VulkanShader : public Shader {

public:

    struct VulkanShaderCreateInfo {
        VkDevice device;
        VkRenderPass* pRenderPass;
        VkExtent2D* pExtent;
    };

public:

    VulkanShader(const std::string &vertFilePath, const std::string &fragFilePath,
                 const std::vector<Reference<VulkanDescriptorSetLayout>> &descriptorSetLayouts,
                 const ShaderPipelineInfo &pipelineInfo, const VulkanShaderCreateInfo &createInfo);

    ~VulkanShader();

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

private:

    void create_pipeline_layout(
            const std::vector<Reference<VulkanDescriptorSetLayout>> &descriptorSetLayouts);

    VkShaderModule create_shader_module(const std::vector<uint32_t> &code);
private:

    ShaderPipelineInfo m_pipelineInfo;
    VulkanShaderCreateInfo m_info;
    VertexLayout m_vertexLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkVertexInputBindingDescription m_inputBinding;
    uint32_t m_outputAttachmentCount = 0;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    std::vector<uint32_t> m_vertShaderCode, m_fragShaderCode;

    bool m_cleared;

};

EG_END

#endif //EAGLE_VULKANSHADER_H
