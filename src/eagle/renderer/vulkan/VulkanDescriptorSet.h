#ifndef EAGLE_VULKANDESCRIPTORSET_H
#define EAGLE_VULKANDESCRIPTORSET_H

#include "eagle/renderer/DescriptorSet.h"
#include "eagle/renderer/vulkan/VulkanCore.h"
#include "eagle/renderer/vulkan/VulkanShader.h"
#include "eagle/renderer/vulkan/VulkanUniformBuffer.h"
#include "eagle/renderer/vulkan/VulkanTexture2D.h"

_EAGLE_BEGIN

struct VulkanDescriptorSetCreateInfo {
    VkDevice device;
    uint32_t bufferCount;
};


class VulkanDescriptorSet : public DescriptorSet {
public:
    VulkanDescriptorSet(std::shared_ptr<VulkanShader> shader,
                        const std::vector<std::shared_ptr<VulkanUniformBuffer>> &uniformBuffers,
                        const std::vector<std::shared_ptr<VulkanImage>> &images,
                        VulkanDescriptorSetCreateInfo createInfo);
    ~VulkanDescriptorSet();

    void cleanup();
    void create_descriptor_sets();
    void create_descriptor_pool();
    inline std::vector<VkDescriptorSet>& get_descriptors() {return m_descriptorSets;}
    inline std::weak_ptr<VulkanShader> get_shader() { return m_shader; }
    
private:

    VkDescriptorPool m_descriptorPool;
    std::vector<VkDescriptorSet> m_descriptorSets;
    std::weak_ptr<VulkanShader> m_shader;
    std::vector<std::shared_ptr<VulkanUniformBuffer>> m_uniformBuffers;
    std::vector<std::shared_ptr<VulkanImage>> m_images;
    VulkanDescriptorSetCreateInfo m_info;

    bool m_cleared = true;

};

_EAGLE_END

#endif //EAGLE_VULKANDESCRIPTORSET_H
