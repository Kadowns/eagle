#include "eagle/renderer/vulkan/VulkanDescriptorSet.h"



_EAGLE_BEGIN

VulkanDescriptorSet::VulkanDescriptorSet(std::shared_ptr<VulkanShader> shader,
                                         const std::vector<std::shared_ptr<VulkanUniformBuffer>> &uniformBuffers,
                                         VulkanDescriptorSetCreateInfo createInfo) :
    m_shader(shader), m_uniformBuffers(uniformBuffers), m_info(createInfo){
    create_descriptor_pool();
    create_descriptor_sets();
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    cleanup();
}

void VulkanDescriptorSet::create_descriptor_pool() {

    auto layoutBindings = m_shader.lock()->get_descriptor_set_layout_bindings();

    std::vector<VkDescriptorPoolSize> poolSizes = {};
    poolSizes.resize(layoutBindings.size());
    for (size_t i = 0; i < layoutBindings.size(); i++) {
        poolSizes[i].type = layoutBindings[i].descriptorType;
        poolSizes[i].descriptorCount = m_info.bufferCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(poolSizes[0].descriptorCount);

    VK_CALL_ASSERT(vkCreateDescriptorPool(m_info.device, &poolInfo, nullptr, &m_descriptorPool)) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void VulkanDescriptorSet::create_descriptor_sets() {

    if (!m_cleared) return;

    std::vector<VkDescriptorSetLayout> layouts(m_info.bufferCount, m_shader.lock()->get_descriptor_set_layout());
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_info.bufferCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_info.bufferCount);
    VK_CALL_ASSERT(vkAllocateDescriptorSets(m_info.device, &allocInfo, m_descriptorSets.data())) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    std::vector<VkDescriptorBufferInfo> bufferInfos = {};
    bufferInfos.resize(m_uniformBuffers.size());
    for (size_t i = 0; i < m_descriptorSets.size(); i++) {

        for (size_t j = 0; j < bufferInfos.size(); j++) {
            bufferInfos[j].buffer = m_uniformBuffers[j]->get_buffers()[i]->get_native_buffer();
            bufferInfos[j].offset = 0;
            bufferInfos[j].range = m_uniformBuffers[j]->size();
        }

        std::vector<VkWriteDescriptorSet> descriptorWrite = {};

        descriptorWrite.resize(bufferInfos.size());
        for (size_t j = 0; j < bufferInfos.size(); j++) {
            descriptorWrite[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite[j].dstSet = m_descriptorSets[i];
            descriptorWrite[j].dstBinding = j;
            descriptorWrite[j].dstArrayElement = 0;
            descriptorWrite[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite[j].descriptorCount = 1;
            descriptorWrite[j].pBufferInfo = &bufferInfos[j];
        }

        VK_CALL vkUpdateDescriptorSets(m_info.device, static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);
    }
    m_cleared = false;
}

void VulkanDescriptorSet::bind() {
    VK_CALL vkCmdBindDescriptorSets(m_drawInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_shader.lock()->get_layout(), 0, 1, &m_descriptorSets[m_drawInfo.bufferIndex], 0, nullptr);
}

void VulkanDescriptorSet::set_draw_info(VulkanDescriptorSetDrawInfo drawInfo) {
    m_drawInfo = drawInfo;
}

void VulkanDescriptorSet::cleanup() {
    if (m_cleared) return;
    VK_CALL vkDestroyDescriptorPool(m_info.device, m_descriptorPool, nullptr);
    m_cleared = true;
}

_EAGLE_END


