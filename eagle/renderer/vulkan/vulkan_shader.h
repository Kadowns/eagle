#ifndef EAGLE_VULKANSHADER_H
#define EAGLE_VULKANSHADER_H


#include <eagle/renderer/shader.h>
#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set_layout.h>
#include <eagle/events/event.h>

namespace eagle {

struct VulkanShaderCreateInfo {
    VkDevice device = VK_NULL_HANDLE;
    VkExtent2D extent = {0, 0};
    ImmediateEvent<VkExtent2D>* on_window_resized = nullptr;
};

class VulkanShader : public Shader {
public:

    VulkanShader(const ShaderCreateInfo &createInfo, const VulkanShaderCreateInfo &nativeCreateInfo);

    ~VulkanShader();

    void create_pipeline();
    void cleanup_pipeline();

    DescriptorSetLayout* descriptor_set_layout(uint32_t index) const override;

    VkPipeline native_pipeline();

    VkPipelineLayout native_pipeline_layout();

private:

    void create_pipeline_layout();

private:

    VulkanShaderCreateInfo m_nativeCreateInfo;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
    uint32_t m_outputAttachmentCount = 0;
    std::vector<VkVertexInputBindingDescription> m_inputBindings;
    std::vector<std::unique_ptr<VulkanDescriptorSetLayout>> m_descriptorSetLayouts;
    std::vector<VkVertexInputAttributeDescription> m_inputAttributes;
    EventListener m_listener;
    bool m_cleared = true;

};

}

#endif //EAGLE_VULKANSHADER_H
