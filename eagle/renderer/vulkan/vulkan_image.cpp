//
// Created by Ricardo on 10/25/2020.
//

#include <eagle/renderer/vulkan/vulkan_image.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>

namespace eagle {

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo &nativeCreateInfo) :
    Image(imageCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo) {
    EG_TRACE("eagle","Creating a vulkan image!");
    create();
    EG_TRACE("eagle","Vulkan image created!");
}

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, std::vector<VkImage> images) :
        Image(imageCreateInfo),
        m_nativeCreateInfo(nativeCreateInfo),
        m_images(std::move(images)),
        m_createdFromExternalImage(true){
    EG_TRACE("eagle","Creating a vulkan image from a swapchain image!");

    m_descriptorType = DescriptorType::SAMPLED_IMAGE;

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.aspectMask = VulkanConverter::to_vk_flags<VkImageAspectFlags>(m_createInfo.aspects);

    m_views.resize(m_images.size());
    for (int i = 0; i < m_views.size(); i++) {
        VK_CALL
        VulkanHelper::create_image_view(
                m_nativeCreateInfo.device,
                m_images[i],
                m_views[i],
                VulkanConverter::to_vk(m_createInfo.format),
                VK_IMAGE_VIEW_TYPE_2D,
                subresourceRange
        );
    }

    EG_TRACE("eagle","Vulkan image created from a swapchain image!");
}

VulkanImage::~VulkanImage() {
    EG_TRACE("eagle","Destroying a vulkan image!");
    clear();
    EG_TRACE("eagle","Vulkan image destroyed!");
}

void VulkanImage::on_resize() {
    EG_TRACE("eagle","Resizing a vulkan image!");
    clear();
    create();
    EG_TRACE("eagle","Vulkan image resized!");
}

void VulkanImage::create() {
    EG_TRACE("eagle","Creating a vulkan image!");
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_createInfo.width;
    imageInfo.extent.height = m_createInfo.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = m_createInfo.mipLevels;
    imageInfo.arrayLayers = m_createInfo.arrayLayers;
    imageInfo.format = VulkanConverter::to_vk(m_createInfo.format);
    imageInfo.tiling = VulkanConverter::to_vk(m_createInfo.tiling);
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VulkanConverter::to_vk_flags<VkImageUsageFlags>(m_createInfo.usages);
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (m_createInfo.type == ImageType::CUBE){
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }


    if (imageInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT){
        assert(m_createInfo.width == m_createInfo.height && "Image width and height must be equal when creating a cube map");
        assert(m_createInfo.arrayLayers >= 6 && "Image array layers must be equal or greater than 6 when creating a cube map");
    }

    m_images.resize(m_nativeCreateInfo.imageCount);
    for (int i = 0; i < m_nativeCreateInfo.imageCount; i++) {
        VK_CALL_ASSERT(vkCreateImage(m_nativeCreateInfo.device, &imageInfo, nullptr, &m_images[i])) {
            throw std::runtime_error("failed to create image!");
        }
    }

    m_descriptorType = imageInfo.usage & VK_IMAGE_USAGE_STORAGE_BIT ? DescriptorType::STORAGE_IMAGE : DescriptorType::SAMPLED_IMAGE;


    VkMemoryRequirements memRequirements;
    VK_CALL vkGetImageMemoryRequirements(m_nativeCreateInfo.device, m_images[0], &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            m_nativeCreateInfo.physicalDevice,
            memRequirements.memoryTypeBits,
            VulkanConverter::to_vk_flags<VkMemoryPropertyFlags>(m_createInfo.memoryProperties)
    );

    m_memories.resize(m_images.size());
    for (int i = 0; i < m_memories.size(); i++) {
        VK_CALL_ASSERT(vkAllocateMemory(m_nativeCreateInfo.device, &allocInfo, nullptr, &m_memories[i])) {
            throw std::runtime_error("failed to allocate image memory!");
        }
        VK_CALL vkBindImageMemory(m_nativeCreateInfo.device, m_images[i], m_memories[i], 0);
    }

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = m_createInfo.arrayLayers;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.aspectMask = VulkanConverter::to_vk_flags<VkImageAspectFlags>(m_createInfo.aspects);

    m_views.resize(m_memories.size());
    for (int i = 0; i < m_views.size(); i++) {
        if (!m_createInfo.buffer.empty()) {
            copy_buffer_data_to_image(subresourceRange, i);
        } else {
            VK_CALL VulkanHelper::transition_image_layout(
                    m_nativeCreateInfo.device,
                    m_nativeCreateInfo.commandPool,
                    m_nativeCreateInfo.graphicsQueue,
                    m_images[i],
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VulkanConverter::to_vk(m_createInfo.layout),
                    subresourceRange
            );
        }

        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
        if (m_createInfo.type == ImageType::CUBE){
            viewType = VK_IMAGE_VIEW_TYPE_CUBE;
        }

        VK_CALL VulkanHelper::create_image_view(
                m_nativeCreateInfo.device,
                m_images[i],
                m_views[i],
                VulkanConverter::to_vk(m_createInfo.format),
                viewType,
                subresourceRange
        );
    }
    EG_TRACE("eagle","Vulkan image created!");
}

void VulkanImage::copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange, uint32_t index) {
    EG_TRACE("eagle","Copying buffer to image!");
    std::shared_ptr<VulkanBuffer> stagingBuffer;

    VulkanBufferCreateInfo bufferInfo = {};
    bufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VK_CALL VulkanBuffer::create_buffer(
            m_nativeCreateInfo.physicalDevice,
            m_nativeCreateInfo.device,
            stagingBuffer,
            bufferInfo,
            m_createInfo.buffer.size(),
            m_createInfo.buffer.data()
    );


    VK_CALL VulkanHelper::transition_image_layout(
            m_nativeCreateInfo.device,
            m_nativeCreateInfo.commandPool,
            m_nativeCreateInfo.graphicsQueue,
            m_images[index],
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresourceRange,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(m_nativeCreateInfo.device,
                                                                             m_nativeCreateInfo.commandPool);

    VkDeviceSize faceSize = m_createInfo.width * m_createInfo.height * format_size(m_createInfo.format);
    VkDeviceSize bufferOffset = 0;
    std::vector<VkBufferImageCopy> copyRegions = {};
    copyRegions.reserve(m_createInfo.arrayLayers);
    for (uint32_t layerIndex = 0; layerIndex < m_createInfo.arrayLayers; layerIndex++){
        VkBufferImageCopy region = {};
        region.bufferOffset = bufferOffset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = subresourceRange.aspectMask;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = layerIndex;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
                (uint32_t)m_createInfo.width,
                (uint32_t)m_createInfo.height,
                1
        };
        bufferOffset += faceSize;
        copyRegions.emplace_back(region);
    }


    VK_CALL vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer->native_buffer(),
            m_images[index],
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            copyRegions.size(),
            copyRegions.data()
    );

    VK_CALL VulkanHelper::end_single_time_commnds(
            m_nativeCreateInfo.device,
            m_nativeCreateInfo.commandPool,
            commandBuffer,
            m_nativeCreateInfo.graphicsQueue
    );

    VK_CALL VulkanHelper::transition_image_layout(
            m_nativeCreateInfo.device,
            m_nativeCreateInfo.commandPool,
            m_nativeCreateInfo.graphicsQueue,
            m_images[index],
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VulkanConverter::to_vk(m_createInfo.layout),
            subresourceRange,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    );

    stagingBuffer->destroy();
    EG_TRACE("eagle","Buffer copied to image!");
}

void VulkanImage::clear() {
    EG_TRACE("eagle","Clearing a vulkan image!");
    for (auto& view : m_views){
        if (view){
            VK_CALL vkDestroyImageView(m_nativeCreateInfo.device, view, nullptr);
        }
    }

    if (!m_createdFromExternalImage) {
        for (auto& memory : m_memories){
            if (memory){
                VK_CALL vkFreeMemory(m_nativeCreateInfo.device, memory, nullptr);
            }
        }

        for (auto& image : m_images){
            if (image){
                VK_CALL vkDestroyImage(m_nativeCreateInfo.device, image, nullptr);
            }
        }
    }
    m_views.clear();
    m_memories.clear();
    m_images.clear();
    EG_TRACE("eagle","Vulkan image cleared!");
}

DescriptorType VulkanImage::type() const {
    return m_descriptorType;
}

}
