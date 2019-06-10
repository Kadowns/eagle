#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "VulkanCore.h"
#include "eagle/renderer/Shader.h"
#include "eagle/renderer/ShaderItemLayout.h"

_EAGLE_BEGIN

class VulkanShader : public Shader {

public:

    struct VulkanShaderCreateInfo {
        VkDevice device;
        VkExtent2D extent;
        VkRenderPass renderPass;
    };

    VulkanShader(const std::string &vertFileName, const std::string &fragFileName,
                 const VulkanShaderCreateInfo &createInfo);

    ~VulkanShader();

    inline void set_vulkan_info(const VulkanShaderCreateInfo &info) { m_createInfo = info; }

    void bind_command_buffer(VkCommandBuffer cmd);

    virtual void bind() override;

    virtual void compile() override;

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

private:

    VkShaderModule create_shader_module(const std::vector<char> &code);

    VkDescriptorSetLayoutBinding create_descriptor_set_layout_binding(uint32_t binding,
                                                                      VkDescriptorType type,
                                                                      uint32_t descriptorCount,
                                                                      VkShaderStageFlags flags,
                                                                      const VkSampler *pImmutableSamplers);

    VkFormat component_format(const SHADER_ITEM_COMPONENT& component);

    VkVertexInputBindingDescription get_binding_description(const ShaderItemLayout& layout);
    std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions(const ShaderItemLayout& layout);

    VulkanShaderCreateInfo m_createInfo;

    VkCommandBuffer m_cmd;

    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    VkDescriptorSetLayout m_descriptorSetLayout;
    std::vector<VkDescriptorSetLayoutBinding> m_layoutBindings;

    bool m_cleared;

};

_EAGLE_END

#endif //EAGLE_VULKANSHADER_H
