#include <eagle/core/renderer/vulkan/VulkanStorageBuffer.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>
#include "eagle/core/renderer/vulkan/VulkanDescriptorSet.h"
#include "eagle/core/renderer/vulkan/VulkanImage.h"

EG_BEGIN

VulkanDescriptorSet::VulkanDescriptorSet(const Reference<VulkanDescriptorSetLayout> &descriptorSetLayout,
                                         const std::vector<Reference<DescriptorItem>> &descriptorItems,
                                         const VulkanDescriptorSetCreateInfo& createInfo) :
    m_descriptorSetLayout(descriptorSetLayout), m_descriptorItems(descriptorItems), m_info(createInfo){
    create_descriptor_sets();
    update_descriptor_sets();
}

VulkanDescriptorSet::VulkanDescriptorSet(const Reference<VulkanDescriptorSetLayout> &descriptorSetLayout,
                                         const VulkanDescriptorSetCreateInfo &createInfo) :
    m_descriptorSetLayout(descriptorSetLayout), m_info(createInfo){
    create_descriptor_sets();
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    cleanup();
}

void VulkanDescriptorSet::create_descriptor_sets() {

    if (!m_cleared) return;
    auto layoutBindings = m_descriptorSetLayout.lock()->get_native_bindings();

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

    std::vector<VkDescriptorSetLayout> layouts(m_info.bufferCount, m_descriptorSetLayout.lock()->get_native_layout());
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_info.bufferCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_info.bufferCount);
    VK_CALL_ASSERT(vkAllocateDescriptorSets(m_info.device, &allocInfo, m_descriptorSets.data())) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    m_cleared = false;
}

void VulkanDescriptorSet::update(const std::vector<Reference<DescriptorItem>> &descriptorItems) {


    m_descriptorItems = descriptorItems;

    if (m_cleared) return;

    for (uint32_t i = 0; i < m_descriptorSets.size(); i++){
        m_dirtyDescriptors.insert(i);
    }
    VulkanCleaner::push(this);
}

bool VulkanDescriptorSet::is_dirty() const {
    return !m_dirtyDescriptors.empty();
}

void VulkanDescriptorSet::flush(uint32_t index) {

    std::vector<VkDescriptorSetLayoutBinding> descriptorBindings = m_descriptorSetLayout.lock()->get_native_bindings();
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;

    //foreach descriptor item in descriptor set
    for (uint32_t j = 0; j < m_descriptorItems.size(); j++){

        switch (m_descriptorItems[j]->type()){

            case DescriptorType::UNIFORM_BUFFER:{
                auto buffer = std::static_pointer_cast<VulkanUniformBuffer>(m_descriptorItems[j]);
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = buffer->get_buffers()[index]->native_buffer();
                bufferInfo.offset = 0;
                bufferInfo.range = buffer->size();
                bufferInfos.push_back(bufferInfo);
                break;
            }
            case DescriptorType::STORAGE_BUFFER:{
                auto buffer = std::static_pointer_cast<VulkanStorageBuffer>(m_descriptorItems[j]);
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = buffer->get_buffers()[index]->native_buffer();
                bufferInfo.offset = 0;
                bufferInfo.range = buffer->size();
                bufferInfos.push_back(bufferInfo);
                break;
            }
            case DescriptorType::TEXTURE:{
                auto texture = std::static_pointer_cast<VulkanTexture>(m_descriptorItems[j]);
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(texture->image()->layout());
                imageInfo.imageView = texture->native_image()->native_image_view();
                imageInfo.sampler = texture->sampler();
                imageInfos.push_back(imageInfo);
                break;
            }
            case DescriptorType::IMAGE:{
                auto image = std::static_pointer_cast<VulkanImage>(m_descriptorItems[j]);
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(image->layout());
                imageInfo.imageView = image->native_image_view();
                imageInfos.push_back(imageInfo);
            }break;
            case DescriptorType::STORAGE_IMAGE:{
                auto image = std::static_pointer_cast<VulkanImage>(m_descriptorItems[j]);
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(image->layout());
                imageInfo.imageView = image->native_image_view();
                imageInfos.push_back(imageInfo);
            }break;
        }
    }

    std::vector<VkWriteDescriptorSet> descriptorWrite = {};

    descriptorWrite.resize(m_descriptorItems.size());
    size_t bufferIndex = 0;
    size_t imageIndex = 0;
    for (size_t j = 0; j < descriptorWrite.size(); j++) {
        descriptorWrite[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[j].dstSet = m_descriptorSets[index];
        descriptorWrite[j].dstBinding = j;
        descriptorWrite[j].dstArrayElement = 0;
        descriptorWrite[j].descriptorType = descriptorBindings[j].descriptorType;
        descriptorWrite[j].descriptorCount = 1;
        if (descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER ||
            descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
            ){
            descriptorWrite[j].pBufferInfo = &bufferInfos[bufferIndex];
            bufferIndex++;
        }
        else if (descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                 descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                 descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
                 ){
            descriptorWrite[j].pImageInfo = &imageInfos[imageIndex];
            imageIndex++;
        }
    }

    VK_CALL vkUpdateDescriptorSets(m_info.device, static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);

    m_dirtyDescriptors.erase(index);
}

void VulkanDescriptorSet::update_descriptor_sets() {

    if (m_cleared) return;


    for (uint32_t i = 0; i < m_descriptorSets.size(); i++){
        flush(i);
    }
}


void VulkanDescriptorSet::cleanup() {
    if (m_cleared) return;
    VK_CALL vkDestroyDescriptorPool(m_info.device, m_descriptorPool, nullptr);
    m_cleared = true;
}



EG_END


