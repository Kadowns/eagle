//
// Created by Ricardo on 24/08/2022.
//

#include <eagle/renderer/vulkan/vulkan_resource.h>
#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanDeviceAllocator::VulkanDeviceAllocator(VkDevice device, size_t frameCount) : m_device(device), m_frames(frameCount){}

VulkanDeviceAllocator::~VulkanDeviceAllocator() {

}

void VulkanDeviceAllocator::clean_frame(size_t frameIndex){
    auto& frame = m_frames[frameIndex];

    //buffers
    for (auto& buffer : frame.buffers){
        vkDestroyBuffer(m_device, buffer, nullptr);
    }
    frame.buffers.clear();

}

VulkanResource<VkBuffer> VulkanDeviceAllocator::create_buffer(const VkBufferCreateInfo &createInfo) {
    std::vector<VkBuffer> buffers(m_frames.size(), VK_NULL_HANDLE);
    for (auto& buffer : buffers){
        auto result = vkCreateBuffer(m_device, &createInfo, nullptr, &buffer);
        if (result != VK_SUCCESS){
            throw VulkanException("Failed to create vulkan buffer", result);
        }
    }
    return {this, std::move(buffers)};
}

void VulkanDeviceAllocator::destroy(VkBuffer buffer, size_t frameIndex){
    m_frames[frameIndex].buffers.emplace_back(buffer);
}

}
