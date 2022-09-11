//
// Created by Novak on 02/06/2019.
//

#ifndef EAGLE_VULKANHELPER_H
#define EAGLE_VULKANHELPER_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>
#include <eagle/renderer/vulkan/vulkan_command_queue.h>

#include <vector>

namespace eagle {

class VulkanSharedCommandPool;

class VulkanHelper {

public:

    static void
    create_image_view(VkDevice device, VkImage image, VkImageView &imageView, VkFormat format, VkImageViewType viewType,
                      VkImageSubresourceRange subresourceRange);

    static uint32_t
    find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    static void
    create_image_sampler(VkDevice device, VkSampler& sampler, VkSamplerAddressMode wrapMode, VkFilter filter,
                         uint32_t mipLevels);

    static VkCommandBuffer
    begin_single_time_commands(VulkanCommandQueue* queue);

    static void
    end_single_time_commnds(VulkanCommandQueue* queue, VkCommandBuffer commandBuffer);

    static void
    create_image(VkPhysicalDevice physicalDevice, VkDevice device, uint32_t width, uint32_t height,
                 uint32_t mipLevels, uint32_t arrayLayers, VkFormat format, VkImageTiling tiling,
                 VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage &image,
                 VkDeviceMemory &imageMemory, VkImageCreateFlags flags = 0);

    static void
    transition_image_layout(VulkanCommandQueue* queue, VkImage image,
                            VkImageLayout oldLayout, VkImageLayout newLayout,
                            VkImageSubresourceRange subresourceRange,
                            VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                            VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    static VkFormat find_supported_format(VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    static VkFormat find_depth_format(VkPhysicalDevice physicalDevice);

    static std::shared_ptr<VulkanBuffer>
    create_baked_buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBufferUsageFlags usageFlags,
                        void *data, VkDeviceSize size, VulkanCommandQueue* queue);

    static std::shared_ptr<VulkanBuffer>
    create_dynamic_buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBufferUsageFlagBits bufferUsage,
                          void *data, VkDeviceSize size);

    static void upload_baked_buffer(std::shared_ptr<VulkanBuffer> &buffer, VulkanCommandQueue* queue, void *data);

    static void upload_dynamic_buffer(std::shared_ptr<VulkanBuffer>& buffer, VkDeviceSize size, void* data);

};

}

#endif //EAGLE_VULKANHELPER_H
