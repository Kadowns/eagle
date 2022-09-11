//
// Created by Novak on 02/06/2019.
//

#include <eagle/renderer/vulkan/vulkan_helper.h>
#include <eagle/renderer/vulkan/vulkan_command_queue.h>

namespace eagle {

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

VkCommandBuffer VulkanHelper::begin_single_time_commands(VulkanCommandQueue* queue) {

    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    queue->allocate(commandBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanHelper::end_single_time_commnds(VulkanCommandQueue* queue, VkCommandBuffer commandBuffer) {

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    queue->submit(submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(queue->native_queue());

    queue->free(commandBuffer);
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

void VulkanHelper::transition_image_layout(
        VulkanCommandQueue* queue,
        VkImage image,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        VkImageSubresourceRange subresourceRange,
        VkPipelineStageFlags srcStage,
        VkPipelineStageFlags dstStage) {
    VkCommandBuffer commandBuffer = begin_single_time_commands(queue);

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange = subresourceRange;

    switch (oldLayout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            // Image layout is undefined (or does not matter)
            // Only valid as initial layout
            // No flags required, listed only for completeness
            barrier.srcAccessMask = 0;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            // Image is preinitialized
            // Only valid as initial layout for linear images, preserves memory contents
            // Make sure host writes have been finished
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            // Image is a color attachment
            // Make sure any writes to the color buffer have been finished
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            // Image is a depth/stencil attachment
            // Make sure any writes to the depth/stencil buffer have been finished
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            // Image is a transfer source
            // Make sure any reads from the image have been finished
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            // Image is a transfer destination
            // Make sure any writes to the image have been finished
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            // Image is read by a shader
            // Make sure any shader reads from the image have been finished
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            // Other source layouts aren't handled (yet)
            throw std::logic_error("image source layout is not handled");
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (newLayout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            // Image will be used as a transfer destination
            // Make sure any writes to the image have been finished
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            // Image will be used as a transfer source
            // Make sure any reads from the image have been finished
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            // Image will be used as a color attachment
            // Make sure any writes to the color buffer have been finished
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            // Image layout will be used as a depth/stencil attachment
            // Make sure any writes to depth/stencil buffer have been finished
            barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            // Image will be read in a shader (sampler, input attachment)
            // Make sure any writes to the image have been finished
            if (barrier.srcAccessMask == 0) {
                barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            }
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            // Other source layouts aren't handled (yet)
            EG_WARNING("eagle", "New image layout not handled!");
            break;
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            srcStage, dstStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    end_single_time_commnds(queue, commandBuffer);
}

void
VulkanHelper::create_image_sampler(VkDevice device, VkSampler& sampler, VkSamplerAddressMode wrapMode, VkFilter filter,
                                   uint32_t mipLevels) {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
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
    samplerInfo.maxLod = static_cast<float>(mipLevels);

    if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

VkFormat VulkanHelper::find_supported_format(VkPhysicalDevice physicalDevice, const std::vector<VkFormat> &candidates,
                                        VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

VkFormat VulkanHelper::find_depth_format(VkPhysicalDevice physicalDevice) {
    return find_supported_format(physicalDevice, {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                 VK_IMAGE_TILING_OPTIMAL,
                                 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

std::shared_ptr<VulkanBuffer> VulkanHelper::create_baked_buffer(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkBufferUsageFlags usageFlags,
        void *data,
        VkDeviceSize size,
        VulkanCommandQueue* queue) {

    VulkanBufferCreateInfo stagingBufferCreateInfo = {};
    stagingBufferCreateInfo.physicalDevice = physicalDevice;
    stagingBufferCreateInfo.device = device;
    stagingBufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    stagingBufferCreateInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    stagingBufferCreateInfo.data = data;
    stagingBufferCreateInfo.size = size;

    auto stagingBuffer = std::make_shared<VulkanBuffer>(stagingBufferCreateInfo);

    VulkanBufferCreateInfo bakedBufferCreateInfo = {};
    bakedBufferCreateInfo.physicalDevice = physicalDevice;
    bakedBufferCreateInfo.device = device;
    bakedBufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    bakedBufferCreateInfo.usageFlags = usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bakedBufferCreateInfo.data = nullptr;//will be copied by the staging buffer
    bakedBufferCreateInfo.size = size;

    auto bakedBuffer = std::make_shared<VulkanBuffer>(bakedBufferCreateInfo);

    VulkanBuffer::copy_buffer(
            queue,
            stagingBuffer.get(),
            bakedBuffer.get(),
            size,
            0, 0);

    return bakedBuffer;
}

std::shared_ptr<VulkanBuffer> VulkanHelper::create_dynamic_buffer(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkBufferUsageFlagBits bufferUsage,
        void *data,
        VkDeviceSize size) {

    VulkanBufferCreateInfo createBufferInfo = {};
    createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    createBufferInfo.usageFlags = bufferUsage;
    createBufferInfo.physicalDevice = physicalDevice;
    createBufferInfo.device = device;
    createBufferInfo.data = data;
    createBufferInfo.size = size;

    return std::make_shared<VulkanBuffer>(createBufferInfo);
}

void VulkanHelper::upload_baked_buffer(std::shared_ptr<VulkanBuffer> &buffer, VulkanCommandQueue* queue, void *data) {

    auto& info = buffer->info();

    VulkanBufferCreateInfo statingBufferCreateInfo = {};
    statingBufferCreateInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    statingBufferCreateInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    statingBufferCreateInfo.physicalDevice = info.physicalDevice;
    statingBufferCreateInfo.device = info.device;
    statingBufferCreateInfo.size = info.size;
    statingBufferCreateInfo.data = data;


    auto stagingBuffer = std::make_shared<VulkanBuffer>(statingBufferCreateInfo);

    VulkanBuffer::copy_buffer(
            queue,
            stagingBuffer.get(),
            buffer.get(),
            info.size,
            0, 0);

}

void VulkanHelper::upload_dynamic_buffer(std::shared_ptr<VulkanBuffer>& buffer, VkDeviceSize size, void* data) {
    buffer->map();
    buffer->copy_to(data, size);
    buffer->flush();
    buffer->unmap();
}



}


