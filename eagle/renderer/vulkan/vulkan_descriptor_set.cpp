#include <eagle/renderer/vulkan/vulkan_storage_buffer.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>
#include <eagle/renderer/vulkan/vulkan_descriptor_set.h>
#include <eagle/renderer/vulkan/vulkan_image.h>

namespace eagle {

VulkanDescriptorSet::VulkanDescriptorSet(DescriptorSetCreateInfo info, VulkanDescriptorSetInfo vkInfo) :
    DescriptorSet(std::move(info)),
    m_vkInfo(vkInfo) {
    create_descriptor_sets();
    if (valid()){
        flush_all();
    }
}

VulkanDescriptorSet::~VulkanDescriptorSet() {
    cleanup();
}

void VulkanDescriptorSet::create_descriptor_sets() {

    if (!m_cleared) return;

    auto nativeDescriptorSetLayout = std::static_pointer_cast<VulkanDescriptorSetLayout>(m_info.layout);
    auto layoutBindings = nativeDescriptorSetLayout->native_bindings();

    std::vector<VkDescriptorPoolSize> poolSizes = {};
    poolSizes.resize(layoutBindings.size());
    for (size_t i = 0; i < layoutBindings.size(); i++) {
        poolSizes[i].type = layoutBindings[i].descriptorType;
        poolSizes[i].descriptorCount = m_vkInfo.frameCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(m_vkInfo.frameCount);

    auto result = vkCreateDescriptorPool(m_vkInfo.device, &poolInfo, nullptr, &m_descriptorPool);

    if (result != VK_SUCCESS) {
        throw VulkanException("failed to create descriptor pool", result);
    }

    std::vector<VkDescriptorSetLayout> layouts(m_vkInfo.frameCount, nativeDescriptorSetLayout->native_layout());
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = m_vkInfo.frameCount;
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(m_vkInfo.frameCount);
    result = vkAllocateDescriptorSets(m_vkInfo.device, &allocInfo, m_descriptorSets.data());
    if (result != VK_SUCCESS) {
        throw VulkanException("failed to allocate descriptor sets", result);
    }
    m_cleared = false;
}

bool VulkanDescriptorSet::valid() const {
    return !m_info.descriptors.empty() && std::all_of(m_info.descriptors.begin(), m_info.descriptors.end(), [](Descriptor* descriptor){
        return descriptor != nullptr;
    });
}


bool VulkanDescriptorSet::is_dirty() const {
    return !m_dirtyDescriptors.empty();
}

void VulkanDescriptorSet::flush(uint32_t index) {

    auto nativeDescriptorSetLayout = std::static_pointer_cast<VulkanDescriptorSetLayout>(m_info.layout);

    std::vector<VkDescriptorSetLayoutBinding> descriptorBindings = nativeDescriptorSetLayout->native_bindings();
    std::vector<VkDescriptorBufferInfo> bufferInfos;
    std::vector<VkDescriptorImageInfo> imageInfos;

    //foreach descriptor item in descriptor set
    for (auto descriptor : m_info.descriptors){
        switch (descriptor->type()){

            case DescriptorType::UNIFORM_BUFFER:{
                auto buffer = (VulkanUniformBuffer*)descriptor;
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = buffer->buffers()[index]->native_buffer();
                bufferInfo.offset = 0;
                bufferInfo.range = buffer->size();
                bufferInfos.push_back(bufferInfo);
                break;
            }
            case DescriptorType::STORAGE_BUFFER:{
                auto buffer = (VulkanStorageBuffer*)descriptor;
                VkDescriptorBufferInfo bufferInfo = {};
                bufferInfo.buffer = buffer->get_buffers()[index]->native_buffer();
                bufferInfo.offset = 0;
                bufferInfo.range = buffer->size();
                bufferInfos.push_back(bufferInfo);
                break;
            }
            case DescriptorType::SAMPLED_IMAGE:{
                auto view = (VulkanImageView*)descriptor;
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(view->image().layout());
                imageInfo.imageView = view->native_image_view(index);
                imageInfos.push_back(imageInfo);
                break;
            }
            case DescriptorType::STORAGE_IMAGE:{
                auto view = (VulkanImageView*)descriptor;
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(view->image().layout());
                imageInfo.imageView = view->native_image_view(index);
                imageInfos.push_back(imageInfo);
                break;
            }
            case DescriptorType::COMBINED_IMAGE_SAMPLER:{
                auto texture = (VulkanTexture*)descriptor;
                VkDescriptorImageInfo imageInfo = {};
                imageInfo.imageLayout = VulkanConverter::to_vk(texture->native_image()->layout());
                imageInfo.imageView = texture->native_image()->native_view()->native_image_view(index);
                imageInfo.sampler = texture->sampler();
                imageInfos.push_back(imageInfo);
                break;
            }
        }
    }

    std::vector<VkWriteDescriptorSet> descriptorWrite = {};

    descriptorWrite.resize(size());
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
            descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER){
            descriptorWrite[j].pBufferInfo = &bufferInfos[bufferIndex];
            bufferIndex++;
        }
        else if (descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
                 descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
                 descriptorWrite[j].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE){
            descriptorWrite[j].pImageInfo = &imageInfos[imageIndex];
            imageIndex++;
        }
    }

    vkUpdateDescriptorSets(m_vkInfo.device, static_cast<uint32_t>(descriptorWrite.size()), descriptorWrite.data(), 0, nullptr);

    m_dirtyDescriptors.erase(index);
}

void VulkanDescriptorSet::cleanup() {
    if (m_cleared) return;
    vkDestroyDescriptorPool(m_vkInfo.device, m_descriptorPool, nullptr);
    m_cleared = true;
}

void VulkanDescriptorSet::recreate(uint32_t bufferCount) {
    if (!m_cleared) {
        return;
    }
    m_vkInfo.frameCount = bufferCount;
    create_descriptor_sets();
    for (uint32_t i = 0; i < m_descriptorSets.size(); i++){
        flush(i);
    }
}

void VulkanDescriptorSet::update() {
    if (m_cleared) return;

    for (uint32_t i = 0; i < m_descriptorSets.size(); i++){
        m_dirtyDescriptors.insert(i);
    }
    VulkanCleaner::push(this);
}

void VulkanDescriptorSet::flush_all() {
    for (uint32_t i = 0; i < m_descriptorSets.size(); i++){
        flush(i);
    }
}

VkDescriptorSet VulkanDescriptorSet::native_descriptor_set(uint32_t frameIndex) {
    return m_descriptorSets[frameIndex];
}

}
