//
// Created by Novak on 02/06/2019.
//


#include "eagle/renderer/vulkan/VulkanHelper.h"

_EAGLE_BEGIN

void VulkanHelper::create_image_view(VkDevice device, VkImage image, VkImageView &imageView, VkFormat format,
                                     VkImageViewType viewType,
                                     VkImageSubresourceRange subresourceRange) {

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = viewType;
    viewInfo.format = format;
    viewInfo.subresourceRange = subresourceRange;

    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

uint32_t
VulkanHelper::find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

VkCommandBuffer VulkanHelper::begin_single_time_commands(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanHelper::end_single_time_commnds(
        VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue graphicsQueue) {

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void
VulkanHelper::create_image(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height,
                           uint32_t mipLevels, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
                           VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                           VkDeviceMemory &imageMemory, VkImageCreateFlags flags) {

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = flags;

    if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(device, image, imageMemory, 0);
}

void
VulkanHelper::transition_image_layout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image,
                                      VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                                      VkImageSubresourceRange subresourceRange) {
    VkCommandBuffer commandBuffer = begin_single_time_commands(device, commandPool);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = subresourceRange;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
               newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    end_single_time_commnds(device, commandPool, commandBuffer, graphicsQueue);
}

void
VulkanHelper::create_image_sampler(VkDevice device, VkSampler &sampler, VkSamplerAddressMode wrapMode) {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = wrapMode;
    samplerInfo.addressModeV = wrapMode;
    samplerInfo.addressModeW = wrapMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkFormat VulkanHelper::get_vk_format(EG_FORMAT format) {
    switch (format) {
        case EG_FORMAT_UNDEFINED:
            return VK_FORMAT_UNDEFINED;
        case EG_FORMAT_R4G4_UNORM_PACK8:
            return VK_FORMAT_R4G4_UNORM_PACK8;
        case EG_FORMAT_R4G4B4A4_UNORM_PACK16:
            return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case EG_FORMAT_B4G4R4A4_UNORM_PACK16:
            return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
        case EG_FORMAT_R5G6B5_UNORM_PACK16:
            return VK_FORMAT_R5G6B5_UNORM_PACK16;
        case EG_FORMAT_B5G6R5_UNORM_PACK16:
            return VK_FORMAT_B5G6R5_UNORM_PACK16;
        case EG_FORMAT_R5G5B5A1_UNORM_PACK16:
            return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
        case EG_FORMAT_B5G5R5A1_UNORM_PACK16:
            return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
        case EG_FORMAT_A1R5G5B5_UNORM_PACK16:
            return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
        case EG_FORMAT_R8_UNORM:
            return VK_FORMAT_R8_UNORM;
        case EG_FORMAT_R8_SNORM:
            return VK_FORMAT_R8_SNORM;
        case EG_FORMAT_R8_USCALED:
            return VK_FORMAT_R8_USCALED;
        case EG_FORMAT_R8_SSCALED:
            return VK_FORMAT_R8_SSCALED;
        case EG_FORMAT_R8_UINT:
            return VK_FORMAT_R8_UINT;
        case EG_FORMAT_R8_SINT:
            return VK_FORMAT_R8_SINT;
        case EG_FORMAT_R8_SRGB:
            return VK_FORMAT_R8_SRGB;
        case EG_FORMAT_R8G8_UNORM:
            return VK_FORMAT_R8G8_UNORM;
        case EG_FORMAT_R8G8_SNORM:
            return VK_FORMAT_R8G8_SNORM;
        case EG_FORMAT_R8G8_USCALED:
            return VK_FORMAT_R8G8_USCALED;
        case EG_FORMAT_R8G8_SSCALED:
            return VK_FORMAT_R8G8_SSCALED;
        case EG_FORMAT_R8G8_UINT:
            return VK_FORMAT_R8G8_UINT;
        case EG_FORMAT_R8G8_SINT:
            return VK_FORMAT_R8G8_SINT;
        case EG_FORMAT_R8G8_SRGB:
            return VK_FORMAT_R8G8_SRGB;
        case EG_FORMAT_R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case EG_FORMAT_R8G8B8_SNORM:
            return VK_FORMAT_R8G8B8_SNORM;
        case EG_FORMAT_R8G8B8_USCALED:
            return VK_FORMAT_R8G8B8_USCALED;
        case EG_FORMAT_R8G8B8_SSCALED:
            return VK_FORMAT_R8G8B8_SSCALED;
        case EG_FORMAT_R8G8B8_UINT:
            return VK_FORMAT_R8G8B8_UINT;
        case EG_FORMAT_R8G8B8_SINT:
            return VK_FORMAT_R8G8B8_SINT;
        case EG_FORMAT_R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_SRGB;
        case EG_FORMAT_B8G8R8_UNORM:
            return VK_FORMAT_B8G8R8_UNORM;
        case EG_FORMAT_B8G8R8_SNORM:
            return VK_FORMAT_B8G8R8_SNORM;
        case EG_FORMAT_B8G8R8_USCALED:
            return VK_FORMAT_B8G8R8_USCALED;
        case EG_FORMAT_B8G8R8_SSCALED:
            return VK_FORMAT_B8G8R8_SSCALED;
        case EG_FORMAT_B8G8R8_UINT:
            return VK_FORMAT_B8G8R8_UINT;
        case EG_FORMAT_B8G8R8_SINT:
            return VK_FORMAT_B8G8R8_SINT;
        case EG_FORMAT_B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_SRGB;
        case EG_FORMAT_R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case EG_FORMAT_R8G8B8A8_SNORM:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case EG_FORMAT_R8G8B8A8_USCALED:
            return VK_FORMAT_R8G8B8A8_USCALED;
        case EG_FORMAT_R8G8B8A8_SSCALED:
            return VK_FORMAT_R8G8B8A8_SSCALED;
        case EG_FORMAT_R8G8B8A8_UINT:
            return VK_FORMAT_R8G8B8A8_UINT;
        case EG_FORMAT_R8G8B8A8_SINT:
            return VK_FORMAT_R8G8B8A8_SINT;
        case EG_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case EG_FORMAT_B8G8R8A8_UNORM:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case EG_FORMAT_B8G8R8A8_SNORM:
            return VK_FORMAT_B8G8R8A8_SNORM;
        case EG_FORMAT_B8G8R8A8_USCALED:
            return VK_FORMAT_B8G8R8A8_USCALED;
        case EG_FORMAT_B8G8R8A8_SSCALED:
            return VK_FORMAT_B8G8R8A8_SSCALED;
        case EG_FORMAT_B8G8R8A8_UINT:
            return VK_FORMAT_B8G8R8A8_UINT;
        case EG_FORMAT_B8G8R8A8_SINT:
            return VK_FORMAT_B8G8R8A8_SINT;
        case EG_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case EG_FORMAT_A8B8G8R8_UNORM_PACK32:
            return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
        case EG_FORMAT_A8B8G8R8_SNORM_PACK32:
            return VK_FORMAT_A8B8G8R8_SNORM_PACK32;
        case EG_FORMAT_A8B8G8R8_USCALED_PACK32:
            return VK_FORMAT_A8B8G8R8_USCALED_PACK32;
        case EG_FORMAT_A8B8G8R8_SSCALED_PACK32:
            return VK_FORMAT_A8B8G8R8_SSCALED_PACK32;
        case EG_FORMAT_A8B8G8R8_UINT_PACK32:
            return VK_FORMAT_A8B8G8R8_UINT_PACK32;
        case EG_FORMAT_A8B8G8R8_SINT_PACK32:
            return VK_FORMAT_A8B8G8R8_SINT_PACK32;
        case EG_FORMAT_A8B8G8R8_SRGB_PACK32:
            return VK_FORMAT_A8B8G8R8_SRGB_PACK32;
        case EG_FORMAT_A2R10G10B10_UNORM_PACK32:
            return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case EG_FORMAT_A2R10G10B10_SNORM_PACK32:
            return VK_FORMAT_A2R10G10B10_SNORM_PACK32;
        case EG_FORMAT_A2R10G10B10_USCALED_PACK32:
            return VK_FORMAT_A2R10G10B10_USCALED_PACK32;
        case EG_FORMAT_A2R10G10B10_SSCALED_PACK32:
            return VK_FORMAT_A2R10G10B10_SSCALED_PACK32;
        case EG_FORMAT_A2R10G10B10_UINT_PACK32:
            return VK_FORMAT_A2R10G10B10_UINT_PACK32;
        case EG_FORMAT_A2R10G10B10_SINT_PACK32:
            return VK_FORMAT_A2R10G10B10_SINT_PACK32;
        case EG_FORMAT_A2B10G10R10_UNORM_PACK32:
            return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
        case EG_FORMAT_A2B10G10R10_SNORM_PACK32:
            return VK_FORMAT_A2B10G10R10_SNORM_PACK32;
        case EG_FORMAT_A2B10G10R10_USCALED_PACK32:
            return VK_FORMAT_A2B10G10R10_USCALED_PACK32;
        case EG_FORMAT_A2B10G10R10_SSCALED_PACK32:
            return VK_FORMAT_A2B10G10R10_SSCALED_PACK32;
        case EG_FORMAT_A2B10G10R10_UINT_PACK32:
            return VK_FORMAT_A2B10G10R10_UINT_PACK32;
        case EG_FORMAT_A2B10G10R10_SINT_PACK32:
            return VK_FORMAT_A2B10G10R10_SINT_PACK32;
        case EG_FORMAT_R16_UNORM:
            return VK_FORMAT_R16_UNORM;
        case EG_FORMAT_R16_SNORM:
            return VK_FORMAT_R16_SNORM;
        case EG_FORMAT_R16_USCALED:
            return VK_FORMAT_R16_USCALED;
        case EG_FORMAT_R16_SSCALED:
            return VK_FORMAT_R16_SSCALED;
        case EG_FORMAT_R16_UINT:
            return VK_FORMAT_R16_UINT;
        case EG_FORMAT_R16_SINT:
            return VK_FORMAT_R16_SINT;
        case EG_FORMAT_R16_SFLOAT:
            return VK_FORMAT_R16_SFLOAT;
        case EG_FORMAT_R16G16_UNORM:
            return VK_FORMAT_R16G16_UNORM;
        case EG_FORMAT_R16G16_SNORM:
            return VK_FORMAT_R16G16_SNORM;
        case EG_FORMAT_R16G16_USCALED:
            return VK_FORMAT_R16G16_USCALED;
        case EG_FORMAT_R16G16_SSCALED:
            return VK_FORMAT_R16G16_SSCALED;
        case EG_FORMAT_R16G16_UINT:
            return VK_FORMAT_R16G16_UINT;
        case EG_FORMAT_R16G16_SINT:
            return VK_FORMAT_R16G16_SINT;
        case EG_FORMAT_R16G16_SFLOAT:
            return VK_FORMAT_R16G16_SFLOAT;
        case EG_FORMAT_R16G16B16_UNORM:
            return VK_FORMAT_R16G16B16_UNORM;
        case EG_FORMAT_R16G16B16_SNORM:
            return VK_FORMAT_R16G16B16_SNORM;
        case EG_FORMAT_R16G16B16_USCALED:
            return VK_FORMAT_R16G16B16_USCALED;
        case EG_FORMAT_R16G16B16_SSCALED:
            return VK_FORMAT_R16G16B16_SSCALED;
        case EG_FORMAT_R16G16B16_UINT:
            return VK_FORMAT_R16G16B16_UINT;
        case EG_FORMAT_R16G16B16_SINT:
            return VK_FORMAT_R16G16B16_SINT;
        case EG_FORMAT_R16G16B16_SFLOAT:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case EG_FORMAT_R16G16B16A16_UNORM:
            return VK_FORMAT_R16G16B16A16_UNORM;
        case EG_FORMAT_R16G16B16A16_SNORM:
            return VK_FORMAT_R16G16B16A16_SNORM;
        case EG_FORMAT_R16G16B16A16_USCALED:
            return VK_FORMAT_R16G16B16A16_USCALED;
        case EG_FORMAT_R16G16B16A16_SSCALED:
            return VK_FORMAT_R16G16B16A16_SSCALED;
        case EG_FORMAT_R16G16B16A16_UINT:
            return VK_FORMAT_R16G16B16A16_UINT;
        case EG_FORMAT_R16G16B16A16_SINT:
            return VK_FORMAT_R16G16B16A16_SINT;
        case EG_FORMAT_R16G16B16A16_SFLOAT:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case EG_FORMAT_R32_UINT:
            return VK_FORMAT_R32_UINT;
        case EG_FORMAT_R32_SINT:
            return VK_FORMAT_R32_SINT;
        case EG_FORMAT_R32_SFLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case EG_FORMAT_R32G32_UINT:
            return VK_FORMAT_R32G32_UINT;
        case EG_FORMAT_R32G32_SINT:
            return VK_FORMAT_R32G32_SINT;
        case EG_FORMAT_R32G32_SFLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case EG_FORMAT_R32G32B32_UINT:
            return VK_FORMAT_R32G32B32_UINT;
        case EG_FORMAT_R32G32B32_SINT:
            return VK_FORMAT_R32G32B32_SINT;
        case EG_FORMAT_R32G32B32_SFLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case EG_FORMAT_R32G32B32A32_UINT:
            return VK_FORMAT_R32G32B32A32_UINT;
        case EG_FORMAT_R32G32B32A32_SINT:
            return VK_FORMAT_R32G32B32A32_SINT;
        case EG_FORMAT_R32G32B32A32_SFLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case EG_FORMAT_R64_UINT:
            return VK_FORMAT_R64_UINT;
        case EG_FORMAT_R64_SINT:
            return VK_FORMAT_R64_SINT;
        case EG_FORMAT_R64_SFLOAT:
            return VK_FORMAT_R64_SFLOAT;
        case EG_FORMAT_R64G64_UINT:
            return VK_FORMAT_R64G64_UINT;
        case EG_FORMAT_R64G64_SINT:
            return VK_FORMAT_R64G64_SINT;
        case EG_FORMAT_R64G64_SFLOAT:
            return VK_FORMAT_R64G64_SFLOAT;
        case EG_FORMAT_R64G64B64_UINT:
            return VK_FORMAT_R64G64B64_UINT;
        case EG_FORMAT_R64G64B64_SINT:
            return VK_FORMAT_R64G64B64_SINT;
        case EG_FORMAT_R64G64B64_SFLOAT:
            return VK_FORMAT_R64G64B64_SFLOAT;
        case EG_FORMAT_R64G64B64A64_UINT:
            return VK_FORMAT_R64G64B64A64_UINT;
        case EG_FORMAT_R64G64B64A64_SINT:
            return VK_FORMAT_R64G64B64A64_SINT;
        case EG_FORMAT_R64G64B64A64_SFLOAT:
            return VK_FORMAT_R64G64B64A64_SFLOAT;
        case EG_FORMAT_B10G11R11_UFLOAT_PACK32:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case EG_FORMAT_E5B9G9R9_UFLOAT_PACK32:
            return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
        case EG_FORMAT_D16_UNORM:
            return VK_FORMAT_D16_UNORM;
        case EG_FORMAT_X8_D24_UNORM_PACK32:
            return VK_FORMAT_X8_D24_UNORM_PACK32;
        case EG_FORMAT_D32_SFLOAT:
            return VK_FORMAT_D32_SFLOAT;
        case EG_FORMAT_S8_UINT:
            return VK_FORMAT_S8_UINT;
        case EG_FORMAT_D16_UNORM_S8_UINT:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case EG_FORMAT_D24_UNORM_S8_UINT:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case EG_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
        case EG_FORMAT_BC1_RGB_UNORM_BLOCK:
            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case EG_FORMAT_BC1_RGB_SRGB_BLOCK:
            return VK_FORMAT_BC1_RGB_SRGB_BLOCK;
        case EG_FORMAT_BC1_RGBA_UNORM_BLOCK:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case EG_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
        case EG_FORMAT_BC2_UNORM_BLOCK:
            return VK_FORMAT_BC2_UNORM_BLOCK;
        case EG_FORMAT_BC2_SRGB_BLOCK:
            return VK_FORMAT_BC2_SRGB_BLOCK;
        case EG_FORMAT_BC3_UNORM_BLOCK:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case EG_FORMAT_BC3_SRGB_BLOCK:
            return VK_FORMAT_BC3_SRGB_BLOCK;
        case EG_FORMAT_BC4_UNORM_BLOCK:
            return VK_FORMAT_BC4_UNORM_BLOCK;
        case EG_FORMAT_BC4_SNORM_BLOCK:
            return VK_FORMAT_BC4_SNORM_BLOCK;
        case EG_FORMAT_BC5_UNORM_BLOCK:
            return VK_FORMAT_BC5_UNORM_BLOCK;
        case EG_FORMAT_BC5_SNORM_BLOCK:
            return VK_FORMAT_BC5_SNORM_BLOCK;
        case EG_FORMAT_BC6H_UFLOAT_BLOCK:
            return VK_FORMAT_BC6H_UFLOAT_BLOCK;
        case EG_FORMAT_BC6H_SFLOAT_BLOCK:
            return VK_FORMAT_BC6H_SFLOAT_BLOCK;
        case EG_FORMAT_BC7_UNORM_BLOCK:
            return VK_FORMAT_BC7_UNORM_BLOCK;
        case EG_FORMAT_BC7_SRGB_BLOCK:
            return VK_FORMAT_BC7_SRGB_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case EG_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;
        case EG_FORMAT_EAC_R11_UNORM_BLOCK:
            return VK_FORMAT_EAC_R11_UNORM_BLOCK;
        case EG_FORMAT_EAC_R11_SNORM_BLOCK:
            return VK_FORMAT_EAC_R11_SNORM_BLOCK;
        case EG_FORMAT_EAC_R11G11_UNORM_BLOCK:
            return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
        case EG_FORMAT_EAC_R11G11_SNORM_BLOCK:
            return VK_FORMAT_EAC_R11G11_SNORM_BLOCK;
        case EG_FORMAT_ASTC_4x4_UNORM_BLOCK:
            return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case EG_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return VK_FORMAT_ASTC_4x4_SRGB_BLOCK;
        case EG_FORMAT_ASTC_5x4_UNORM_BLOCK:
            return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case EG_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return VK_FORMAT_ASTC_5x4_SRGB_BLOCK;
        case EG_FORMAT_ASTC_5x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case EG_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_5x5_SRGB_BLOCK;
        case EG_FORMAT_ASTC_6x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case EG_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_6x5_SRGB_BLOCK;
        case EG_FORMAT_ASTC_6x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case EG_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_6x6_SRGB_BLOCK;
        case EG_FORMAT_ASTC_8x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case EG_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x5_SRGB_BLOCK;
        case EG_FORMAT_ASTC_8x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case EG_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x6_SRGB_BLOCK;
        case EG_FORMAT_ASTC_8x8_UNORM_BLOCK:
            return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case EG_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x8_SRGB_BLOCK;
        case EG_FORMAT_ASTC_10x5_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case EG_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x5_SRGB_BLOCK;
        case EG_FORMAT_ASTC_10x6_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case EG_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x6_SRGB_BLOCK;
        case EG_FORMAT_ASTC_10x8_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case EG_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x8_SRGB_BLOCK;
        case EG_FORMAT_ASTC_10x10_UNORM_BLOCK:
            return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case EG_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x10_SRGB_BLOCK;
        case EG_FORMAT_ASTC_12x10_UNORM_BLOCK:
            return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case EG_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return VK_FORMAT_ASTC_12x10_SRGB_BLOCK;
        case EG_FORMAT_ASTC_12x12_UNORM_BLOCK:
            return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        case EG_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return VK_FORMAT_ASTC_12x12_SRGB_BLOCK;
        case EG_FORMAT_G8B8G8R8_422_UNORM:
            return VK_FORMAT_G8B8G8R8_422_UNORM;
        case EG_FORMAT_B8G8R8G8_422_UNORM:
            return VK_FORMAT_B8G8R8G8_422_UNORM;
        case EG_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
            return VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM;
        case EG_FORMAT_G8_B8R8_2PLANE_420_UNORM:
            return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
        case EG_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
            return VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM;
        case EG_FORMAT_G8_B8R8_2PLANE_422_UNORM:
            return VK_FORMAT_G8_B8R8_2PLANE_422_UNORM;
        case EG_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
            return VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM;
        case EG_FORMAT_R10X6_UNORM_PACK16:
            return VK_FORMAT_R10X6_UNORM_PACK16;
        case EG_FORMAT_R10X6G10X6_UNORM_2PACK16:
            return VK_FORMAT_R10X6G10X6_UNORM_2PACK16;
        case EG_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
            return VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16;
        case EG_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
            return VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16;
        case EG_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
            return VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16;
        case EG_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
            return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16;
        case EG_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
            return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16;
        case EG_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
            return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16;
        case EG_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
            return VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16;
        case EG_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
            return VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16;
        case EG_FORMAT_R12X4_UNORM_PACK16:
            return VK_FORMAT_R12X4_UNORM_PACK16;
        case EG_FORMAT_R12X4G12X4_UNORM_2PACK16:
            return VK_FORMAT_R12X4G12X4_UNORM_2PACK16;
        case EG_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
            return VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16;
        case EG_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
            return VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16;
        case EG_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
            return VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16;
        case EG_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
            return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16;
        case EG_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
            return VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16;
        case EG_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
            return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16;
        case EG_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
            return VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16;
        case EG_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
            return VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16;
        case EG_FORMAT_G16B16G16R16_422_UNORM:
            return VK_FORMAT_G16B16G16R16_422_UNORM;
        case EG_FORMAT_B16G16R16G16_422_UNORM:
            return VK_FORMAT_B16G16R16G16_422_UNORM;
        case EG_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
            return VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM;
        case EG_FORMAT_G16_B16R16_2PLANE_420_UNORM:
            return VK_FORMAT_G16_B16R16_2PLANE_420_UNORM;
        case EG_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
            return VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM;
        case EG_FORMAT_G16_B16R16_2PLANE_422_UNORM:
            return VK_FORMAT_G16_B16R16_2PLANE_422_UNORM;
        case EG_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
            return VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM;
        case EG_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
        case EG_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
        case EG_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
        case EG_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
        case EG_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG;
        case EG_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG;
        case EG_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG;
        case EG_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG;
        case EG_FORMAT_RANGE_SIZE:
            return VK_FORMAT_RANGE_SIZE;
        case EG_FORMAT_MAX_ENUM:
            return VK_FORMAT_MAX_ENUM;
    }
}


_EAGLE_END


