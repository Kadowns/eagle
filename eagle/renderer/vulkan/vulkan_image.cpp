//
// Created by Ricardo on 10/25/2020.
//

#include <eagle/renderer/vulkan/vulkan_image.h>
#include <eagle/renderer/vulkan/vulkan_converter.h>
#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

#include <memory>

namespace eagle {

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo &nativeCreateInfo) :
    Image(imageCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo) {
    create();
}

VulkanImage::VulkanImage(const ImageCreateInfo &imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, std::vector<VkImage> images) :
        Image(imageCreateInfo),
        m_nativeCreateInfo(nativeCreateInfo),
        m_images(std::move(images)),
        m_createdFromExternalImage(true){

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.aspectMask = VulkanConverter::eg_flags_to_vk_flags<ImageAspect>(m_createInfo.aspects);

    VulkanImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.image = this;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.descriptorType = DescriptorType::SAMPLED_IMAGE;
    viewCreateInfo.imageCount = m_images.size();
    viewCreateInfo.subresourceRange = subresourceRange;
    viewCreateInfo.device = m_nativeCreateInfo.device;
    viewCreateInfo.format = VulkanConverter::to_vk(m_createInfo.format);

    m_views.resize(m_images.size());
    for (auto& view : m_views) {
        view = std::make_unique<VulkanImageView>(viewCreateInfo);
    }
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
    imageInfo.usage = VulkanConverter::eg_flags_to_vk_flags<ImageUsage>(m_createInfo.usages);
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (m_createInfo.type == ImageType::CUBE){
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    if (imageInfo.flags & VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT){
        assert(m_createInfo.width == m_createInfo.height && "Image width and height must be equal when creating a cube map");
        assert(m_createInfo.arrayLayers >= 6 && "Image array layers must be equal or greater than 6 when creating a cube map");
    }

    m_images.resize(m_nativeCreateInfo.frameCount);
    for (int i = 0; i < m_nativeCreateInfo.frameCount; i++) {
        auto result = vkCreateImage(m_nativeCreateInfo.device, &imageInfo, nullptr, &m_images[i]);
        if (result != VK_SUCCESS){
            throw VulkanException("failed to create image", result);
        }
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_nativeCreateInfo.device, m_images.front(), &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanHelper::find_memory_type(
            m_nativeCreateInfo.physicalDevice,
            memRequirements.memoryTypeBits,
            VulkanConverter::eg_flags_to_vk_flags<MemoryProperty>(m_createInfo.memoryProperties)
    );

    m_memories.resize(m_images.size());
    for (int i = 0; i < m_memories.size(); i++) {
        auto result = vkAllocateMemory(m_nativeCreateInfo.device, &allocInfo, nullptr, &m_memories[i]);
        if (result != VK_SUCCESS) {
            throw VulkanException("failed to allocate image memory", result);
        }
        vkBindImageMemory(m_nativeCreateInfo.device, m_images[i], m_memories[i], 0);
    }

    VkImageSubresourceRange subresourceRange = {};
    subresourceRange.layerCount = m_createInfo.arrayLayers;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.levelCount = m_createInfo.mipLevels;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.aspectMask = VulkanConverter::eg_flags_to_vk_flags<ImageAspect>(m_createInfo.aspects);

    for (int i = 0; i < m_nativeCreateInfo.frameCount; i++) {
        if (!m_createInfo.buffer.empty()) {
            copy_buffer_data_to_image(subresourceRange, i);
        } else if (m_createInfo.layout != ImageLayout::UNDEFINED){

            VulkanHelper::transition_image_layout(
                    (VulkanCommandQueue*)m_createInfo.owningQueue,
                    m_images[i],
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VulkanConverter::to_vk(m_createInfo.layout),
                    subresourceRange
            );
        }
    }

    VulkanImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.image = this;
    viewCreateInfo.viewType = m_createInfo.type == ImageType::CUBE ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.descriptorType = imageInfo.usage & VK_IMAGE_USAGE_STORAGE_BIT ? DescriptorType::STORAGE_IMAGE : DescriptorType::SAMPLED_IMAGE;
    viewCreateInfo.imageCount = m_images.size();
    viewCreateInfo.subresourceRange = subresourceRange;
    viewCreateInfo.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    viewCreateInfo.device = m_nativeCreateInfo.device;
    viewCreateInfo.format = VulkanConverter::to_vk(m_createInfo.format);

    m_views.resize(m_createInfo.mipLevels);
    for (uint32_t i = 0; i < m_views.size(); i++) {
        viewCreateInfo.subresourceRange.baseMipLevel = i;
        m_views[i] = std::make_unique<VulkanImageView>(viewCreateInfo);
    }
    EG_TRACE("eagle","Vulkan image created!");
}

void VulkanImage::copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange, uint32_t index) {

    auto queue = (VulkanCommandQueue*)m_createInfo.owningQueue;

    VulkanBufferCreateInfo bufferInfo = {};
    bufferInfo.physicalDevice = m_nativeCreateInfo.physicalDevice;
    bufferInfo.device = m_nativeCreateInfo.device;
    bufferInfo.data = m_createInfo.buffer.data();
    bufferInfo.size = m_createInfo.buffer.size();
    bufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    auto stagingBuffer = std::make_shared<VulkanBuffer>(bufferInfo);

    VulkanHelper::transition_image_layout(
            queue,
            m_images[index],
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresourceRange,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT
    );

    VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(queue);

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

    vkCmdCopyBufferToImage(
            commandBuffer,
            stagingBuffer->native_buffer(),
            m_images[index],
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            copyRegions.size(),
            copyRegions.data()
    );

    VulkanHelper::end_single_time_commnds(queue, commandBuffer);

    VulkanHelper::transition_image_layout(
            queue,
            m_images[index],
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VulkanConverter::to_vk(m_createInfo.layout),
            subresourceRange,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT
    );
}

void VulkanImage::generate_mipmaps() {

    auto queue = (VulkanCommandQueue*)m_createInfo.owningQueue;

    for (auto image : m_images){

        //First, transition the entire image layout to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
        {
            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.layerCount = m_createInfo.arrayLayers;
            subresourceRange.baseArrayLayer = 0;
            subresourceRange.baseMipLevel = 0;
            subresourceRange.levelCount = m_createInfo.mipLevels;
            subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

            VulkanHelper::transition_image_layout(
                    queue,
                    image,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    subresourceRange
            );
        }

        VkCommandBuffer commandBuffer = VulkanHelper::begin_single_time_commands(queue);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = m_createInfo.arrayLayers;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = m_createInfo.width;
        int32_t mipHeight = m_createInfo.height;

        for (uint32_t i = 1; i < m_createInfo.mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = m_createInfo.arrayLayers;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount =  m_createInfo.arrayLayers;

            vkCmdBlitImage(commandBuffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VulkanConverter::to_vk(m_createInfo.layout);
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            //TODO: maybe allow different pipeline stages?
            vkCmdPipelineBarrier(commandBuffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        // Wait for last mip level blit
        barrier.subresourceRange.baseMipLevel = m_createInfo.mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VulkanConverter::to_vk(m_createInfo.layout);
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);


        VulkanHelper::end_single_time_commnds(queue, commandBuffer);
    }
}

void VulkanImage::clear() {
    EG_TRACE("eagle","Clearing a vulkan image!");

    for (auto& view : m_views){
        view.reset();
    }

    if (!m_createdFromExternalImage) {
        for (auto& memory : m_memories){
            if (memory){
                vkFreeMemory(m_nativeCreateInfo.device, memory, nullptr);
            }
        }

        for (auto& image : m_images){
            if (image){
                vkDestroyImage(m_nativeCreateInfo.device, image, nullptr);
            }
        }
    }
    m_memories.clear();
    m_images.clear();
    EG_TRACE("eagle","Vulkan image cleared!");
}

ImageView* VulkanImage::view(uint32_t mipLevel) {
    return m_views[mipLevel].get();
}

VulkanImageView::VulkanImageView(const VulkanImageViewCreateInfo& createInfo) : m_createInfo(createInfo) {

    m_views.resize(createInfo.imageCount);

    for (uint32_t i = 0; i < m_views.size(); i++){
        VulkanHelper::create_image_view(
                createInfo.device,
                createInfo.image->native_image(i),
                m_views[i],
                createInfo.format,
                createInfo.viewType,
                createInfo.subresourceRange
        );
    }
}

DescriptorType VulkanImageView::type() const {
    return m_createInfo.descriptorType;
}

uint32_t VulkanImageView::mip_level() const {
    return m_createInfo.subresourceRange.baseMipLevel;
}

Image& VulkanImageView::image() const {
    return *m_createInfo.image;
}

VulkanImageView::~VulkanImageView() {
    for (auto& view : m_views){
        if (view){
            vkDestroyImageView(m_createInfo.device, view, nullptr);
        }
    }
}

}
