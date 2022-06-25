//
// Created by Ricardo on 4/10/2022.
//

#include "vulkan_deleter.h"

namespace eagle {

VulkanDeleter::VulkanDeleter(const VulkanDeleterInfo &info) : m_info(info) {

}

void VulkanDeleter::destroy_image(VkImage image, uint32_t frameIndex) {
    m_frames[frameIndex].images.push_back(image);
}

void VulkanDeleter::destroy_image_view(VkImageView view, uint32_t frameIndex) {
    m_frames[frameIndex].views.push_back(view);
}

void VulkanDeleter::destroy_device_memory(VkDeviceMemory memory, uint32_t frameIndex) {
    m_frames[frameIndex].memories.push_back(memory);
}

void VulkanDeleter::destroy_buffer(const std::shared_ptr<VulkanBuffer>& buffer, uint32_t frameIndex) {
    m_frames[frameIndex].buffers.push_back(buffer);
}

void VulkanDeleter::destroy(uint32_t frameIndex) {
    auto &frame = m_frames[frameIndex];

    for (auto view: frame.views) {
        vkDestroyImageView(m_info.device, view, nullptr);
    }
    frame.views.clear();

    for (auto image: frame.images) {
        vkDestroyImage(m_info.device, image, nullptr);
    }
    frame.images.clear();

    frame.buffers.clear();

    for (auto memory: frame.memories) {
        vkFreeMemory(m_info.device, memory, nullptr);
    }
    frame.memories.clear();
}
}