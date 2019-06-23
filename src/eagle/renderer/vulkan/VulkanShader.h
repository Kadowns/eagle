#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "VulkanCore.h"
#include "eagle/renderer/Shader.h"
#include "eagle/renderer/vulkan/VulkanRenderTarget.h"

_EAGLE_BEGIN

class VulkanShader : public Shader {

public:

    struct VulkanShaderCreateInfo {
        VkDevice device;
        VkRenderPass* pRenderPass;
        VkExtent2D* pExtent;
    };

public:

    VulkanShader(const std::string &vertFileName, const std::string &fragFileName,
                 const VulkanShaderCreateInfo &createInfo);

    ~VulkanShader();

    void bind_command_buffer(VkCommandBuffer cmd);

    virtual void bind() override;

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

    const std::vector<VkDescriptorSetLayoutBinding>& get_descriptor_set_layout_bindings();

    VkDescriptorSetLayout const & get_descriptor_set_layout();

private:

    void create_descriptor_set_layout();

    VkShaderModule create_shader_module(const std::vector<uint32_t> &code);

    VkDescriptorSetLayoutBinding create_descriptor_set_layout_binding(uint32_t binding,
                                                                      VkDescriptorType type,
                                                                      uint32_t descriptorCount,
                                                                      VkShaderStageFlags flags,
                                                                      const VkSampler *pImmutableSamplers);

    VkFormat component_format(const SHADER_ITEM_COMPONENT& component);

    VkVertexInputBindingDescription get_binding_description(const ShaderItemLayout& layout);
    std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions(const ShaderItemLayout& layout);

private:

    VulkanShaderCreateInfo m_info;
    VkCommandBuffer m_commandBuffer;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;
    VkVertexInputBindingDescription m_inputBinding;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    std::vector<uint32_t> m_vertShaderCode, m_fragShaderCode;

    bool m_cleared;

};

_EAGLE_END

#endif //EAGLE_VULKANSHADER_H
