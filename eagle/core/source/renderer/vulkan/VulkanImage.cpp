//
// Created by Ricardo on 10/25/2020.
//

#include <eagle/core/renderer/vulkan/VulkanImage.h>
#include <eagle/core/renderer/vulkan/VulkanConverter.h>
#include <eagle/core/renderer/vulkan/VulkanHelper.h>
#include <eagle/core/renderer/vulkan/VulkanBuffer.h>

EG_BEGIN

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo &nativeCreateInfo) :
    Image(imageCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo) {
    EG_CORE_TRACE("Creating a vulkan image!");
    create();
    EG_CORE_TRACE("Vulkan image created!");
}

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, VkImage image) :
    Image(imageCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo),
    m_image(image),
    m_createdFromExternalImage(true){
    EG_CORE_TRACE("Creating a vulkan image from a swapchain image!");

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.aspectMask = VulkanConverter::to_vk_flags<VkImageAspectFlags>(m_createInfo.aspects);

    VK_CALL VulkanHelper::create_image_view(
            m_nativeCreateInfo.device,
            m_image,
            m_view,
            VulkanConverter::to_vk(m_createInfo.format),
            VK_IMAGE_VIEW_TYPE_2D,
            subresourceRange
    );

    EG_CORE_TRACE("Vulkan image created from a swapchain image!");
}

VulkanImage::~VulkanImage() {
    EG_CORE_TRACE("Destroying a vulkan image!");
    clear();
    EG_CORE_TRACE("Vulkan image destroyed!");
}

void VulkanImage::on_resize() {
    EG_CORE_TRACE("Resizing a vulkan image!");
    clear();
    create();
    EG_CORE_TRACE("Vulkan image resized!");
}

void VulkanImage::create() {
    EG_CORE_TRACE("Creating a vulkan image!");
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
    imageInfo.flags = 0;

    VK_CALL_ASSERT(vkCreateImage(m_nativeCreateInfo.device, &imageInfo, nullptr, &m_image)) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    VK_CALL vkGetImageMemoryRequirements(m_nativeCreateInfo.device, m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            m_nativeCreateInfo.physicalDevice,
            memRequirements.memoryTypeBits,
            VulkanConverter::to_vk_flags<VkMemoryPropertyFlags>(m_createInfo.memoryProperties)
    );

    VK_CALL_ASSERT(vkAllocateMemory(m_nativeCreateInfo.device, &allocInfo, nullptr, &m_memory)) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    VK_CALL vkBindImageMemory(m_nativeCreateInfo.device, m_image, m_memory, 0);


    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.aspectMask = VulkanConverter::to_vk_flags<VkImageAspectFlags>(m_createInfo.aspects);

    if (!m_createInfo.bufferData.empty()) {
        copy_buffer_data_to_image(subresourceRange);
    }
    else {
        VK_CALL VulkanHelper::transition_image_layout(
                m_nativeCreateInfo.device,
                m_nativeCreateInfo.commandPool,
                m_nativeCreateInfo.graphicsQueue,
                m_image,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VulkanConverter::to_vk(m_createInfo.layout),
                subresourceRange
        );
    }

    VK_CALL VulkanHelper::create_image_view(
            m_nativeCreateInfo.device,
            m_image,
            m_view,
            VulkanConverter::to_vk(m_createInfo.format),
            VK_IMAGE_VIEW_TYPE_2D,
            subresourceRange
    );
    EG_CORE_TRACE("Vulkan image created!");
}

void VulkanImage::copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange) {
    EG_CORE_TRACE("Copying buffer to image!");
    Reference<VulkanBuffer> stagingBuffer;

    VulkanBufferCreateInfo bufferInfo = {};
    bufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VK_CALL VulkanBuffer::create_buffer(
            m_nativeCreateInfo.physicalDevice,
            m_nativeCreateInfo.device,
            stagingBuffer,
            bufferInfo,
            m_createInfo.bufferData.size(),
            m_createInfo.bufferData.data()
    );


    VK_CALL VulkanHelper::transition_image_layout(
            m_nativeCreateInfo.device,
            m_nativeCreateInfo.commandPool,
            m_nativeCreateInfo.graphicsQueue,
            m_image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresourceRange,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(m_nativeCreateInfo.device,
                                                                             m_nativeCreateInfo.commandPool);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
            (uint32_t)m_createInfo.width,
            (uint32_t)m_createInfo.height,
            1
    };

    VK_CALL vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer->native_buffer(),
            m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
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
            m_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VulkanConverter::to_vk(m_createInfo.layout),
            subresourceRange,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    );

    stagingBuffer->destroy();
    EG_CORE_TRACE("Buffer copied to image!");
}

void VulkanImage::clear() {
    EG_CORE_TRACE("Clearing a vulkan image!");
    if (m_view){
        VK_CALL vkDestroyImageView(m_nativeCreateInfo.device, m_view, nullptr);
    }
    if (!m_createdFromExternalImage) {
        if (m_memory) {
            VK_CALL
            vkFreeMemory(m_nativeCreateInfo.device, m_memory, nullptr);
        }
        if (m_image) {
            VK_CALL
            vkDestroyImage(m_nativeCreateInfo.device, m_image, nullptr);
        }
    }
    m_view = nullptr;
    m_memory = nullptr;
    m_image = nullptr;
    EG_CORE_TRACE("Vulkan image cleared!");
}

EG_END
