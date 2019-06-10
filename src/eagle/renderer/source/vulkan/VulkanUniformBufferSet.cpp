#include "eagle/renderer/vulkan/VulkanUniformBufferSet.h"

_EAGLE_BEGIN

VulkanUniformBufferSet::VulkanUniformBufferSet(const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers)
        : UniformBufferSet(uniformBuffers) {
    create_descriptor_pool();
}

void VulkanUniformBufferSet::create_descriptor_pool() {

    auto layoutBindings = m_material.getShader()->getLayoutBindings();

    std::vector<VkDescriptorPoolSize> poolSizes = {};
    poolSizes.resize(layoutBindings.size());
    for (size_t i = 0; i < layoutBindings.size(); i++) {
        poolSizes[i].type = layoutBindings[i].descriptorType;
        poolSizes[i].descriptorCount = Vulkan::swapchainImages.size();
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(poolSizes[0].descriptorCount);

    if (vkCreateDescriptorPool(Vulkan::device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

_EAGLE_END


