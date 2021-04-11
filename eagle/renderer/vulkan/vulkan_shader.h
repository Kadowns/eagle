#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H

#include "eagle/renderer/shader.h"
#include "vulkan_global_definitions.h"
#include "vulkan_descriptor_set_layout.h"

namespace eagle {

struct VulkanShaderCreateInfo {
    VkDevice device;
    VkExtent2D* pExtent;
};

class VulkanShader : public Shader {
public:

    VulkanShader(const ShaderCreateInfo &createInfo, const VulkanShaderCreateInfo &nativeCreateInfo);

    ~VulkanShader();

    void create_pipeline();
    void cleanup_pipeline();

    const std::vector<std::weak_ptr<DescriptorSetLayout>> get_descriptor_set_layouts() override;
    const std::weak_ptr<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) override;

    VkPipeline &get_pipeline();

    VkPipelineLayout &get_layout();

private:

    void create_pipeline_layout();

private:

    VulkanShaderCreateInfo m_nativeCreateInfo;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
    uint32_t m_outputAttachmentCount = 0;
    std::vector<VkVertexInputBindingDescription> m_inputBindings;
    std::vector<std::shared_ptr<VulkanDescriptorSetLayout>> m_descriptorSetLayouts;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    std::unordered_map<VkShaderStageFlags, std::vector<uint32_t>> m_shaderCodes;

    bool m_cleared;

};

}

#endif //EAGLE_VULKANSHADER_H
