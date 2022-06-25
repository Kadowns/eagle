//
// Created by Ricardo on 4/10/2022.
//

#ifndef EAGLE_VULKAN_DELETER_H
#define EAGLE_VULKAN_DELETER_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>
#include <eagle/renderer/vulkan/vulkan_buffer.h>

#include <memory>
#include <vector>

namespace eagle {

struct VulkanDeleterInfo {
    VkDevice device;
};

class VulkanDeleter {
public:

    VulkanDeleter(const VulkanDeleterInfo& info);

    void destroy_image(VkImage image, uint32_t frameIndex);
    void destroy_image_view(VkImageView view, uint32_t frameIndex);
    void destroy_device_memory(VkDeviceMemory memory, uint32_t frameIndex);
    void destroy_buffer(const std::shared_ptr<VulkanBuffer>& buffer, uint32_t frameIndex);

    void destroy(uint32_t frameIndex);

private:

    struct Frame {
        std::vector<VkDeviceMemory> memories;
        std::vector<VkImage> images;
        std::vector<VkImageView> views;
        std::vector<std::shared_ptr<VulkanBuffer>> buffers;
    };
    VulkanDeleterInfo m_info;
    std::vector<Frame> m_frames;

};

}

#endif //EAGLE_VULKAN_DELETER_H
