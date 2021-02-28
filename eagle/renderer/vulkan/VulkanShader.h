#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "eagle/renderer/Shader.h"
#include "VulkanCore.h"
#include "VulkanDescriptorSetLayout.h"

namespace eagle {

struct VulkanShaderCreateInfo {
    VkDevice device;
    VkExtent2D* pExtent;
};

class VulkanShader : public Shader {
public:

    VulkanShader(const ShaderCreateInfo &createInfo, const VulkanShaderCreateInfo &nativeCreateInfo);

    ~VulkanShader();

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    virtual const std::vector<std::weak_ptr<DescriptorSetLayout>> get_descriptor_set_layouts() override;
    virtual const std::weak_ptr<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

private:

    void create_pipeline_layout();

private:

    ShaderCreateInfo m_createInfo;
    VulkanShaderCreateInfo m_nativeCreateInfo;
    VertexLayout m_vertexLayout;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkVertexInputBindingDescription m_inputBinding;
    uint32_t m_outputAttachmentCount = 0;
    std::vector<std::shared_ptr<VulkanDescriptorSetLayout>> m_descriptorSetLayouts;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    std::unordered_map<VkShaderStageFlags, std::vector<uint32_t>> m_shaderCodes;

    bool m_cleared;

};

}

#endif //EAGLE_VULKANSHADER_H
