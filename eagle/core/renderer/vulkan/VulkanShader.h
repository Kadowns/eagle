#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "eagle/core/renderer/Shader.h"
#include "VulkanCore.h"
//#include "VulkanRenderTarget.h"
#include "VulkanDescriptorSetLayout.h"

EG_BEGIN

class VulkanShader : public Shader {

public:

    struct VulkanShaderCreateInfo {
        VkDevice device;
        VkExtent2D* pExtent;
    };

public:

    VulkanShader(const std::unordered_map<ShaderStage, std::string> &shaderPaths,
                 const ShaderPipelineInfo &pipelineInfo, const VulkanShaderCreateInfo &createInfo);

    ~VulkanShader();

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    virtual const std::vector<Handle<DescriptorSetLayout>> get_descriptor_set_layouts() override;
    virtual const Handle<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

private:

    void create_pipeline_layout();

private:

    ShaderPipelineInfo m_pipelineInfo;
    VulkanShaderCreateInfo m_info;
    VertexLayout m_vertexLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkVertexInputBindingDescription m_inputBinding;
    uint32_t m_outputAttachmentCount = 0;
    std::vector<Reference<VulkanDescriptorSetLayout>> m_descriptorSetLayouts;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    std::unordered_map<VkShaderStageFlags, std::vector<uint32_t>> m_shaderCodes;

    bool m_cleared;

};

EG_END

#endif //EAGLE_VULKANSHADER_H
