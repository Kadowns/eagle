//
// Created by Novak on 22/03/2020.
//

#ifndef EAGLE_VULKANCOMPUTESHADER_H
#define EAGLE_VULKANCOMPUTESHADER_H

#include <eagle/core/renderer/ComputeShader.h>
#include <eagle/core/renderer/vulkan/VulkanCore.h>
#include <eagle/core/renderer/vulkan/VulkanDescriptorSetLayout.h>
#include <eagle/core/renderer/vulkan/VulkanDescriptorSet.h>

EG_BEGIN

struct VulkanComputeShaderCreateInfo{
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue computeQueue;
    uint32_t bufferCount;
    uint32_t* imageIndex;
};

class VulkanComputeShader : public ComputeShader {
public:
    VulkanComputeShader(const std::string &path, const VulkanComputeShaderCreateInfo &createInfo);

    virtual ~VulkanComputeShader();

    virtual void create_pipeline() override;

    virtual void cleanup_pipeline() override;

    virtual void dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ) override;

    virtual void update_descriptor_items(const std::vector<Reference<DescriptorItem>> &descriptorItems) override;

    virtual void set_image(const std::string &name, const Reference<Image> &image) override;

    void clear_descriptor_set();
    void recreate(uint32_t bufferCount);

    inline VkPipeline &get_pipeline() { return m_computePipeline; }

    inline VkPipelineLayout &get_layout() { return m_pipelineLayout; }

private:

    void create_pipeline_layout();
    void create_descriptor_sets();
    void create_command_buffer();
    void create_fence();

private:

    VulkanComputeShaderCreateInfo m_createInfo;
    std::vector<uint32_t> m_code;
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_computePipeline;
    std::vector<DescriptorBindingDescription> m_bindingDescriptions;
    Reference<VulkanDescriptorSetLayout> m_descriptorLayout;
    Reference<VulkanDescriptorSet> m_descriptorSet;
    VkCommandBuffer m_commandBuffer;
    VkFence m_fence;

    bool m_cleared = true;
};



EG_END

#endif //EAGLE_VULKANCOMPUTESHADER_H
