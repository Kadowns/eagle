//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_VULKAN_IMAGE_H
#define EAGLE_VULKAN_IMAGE_H

#include "eagle/renderer/image.h"

#include "vulkan_global_definitions.h"
#include <vector>

namespace eagle {

struct VulkanImageCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    uint32_t imageCount;
};


class VulkanImage : public Image {
public:
    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo);

    //Used for swapchain images
    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, std::vector<VkImage> images);
    virtual ~VulkanImage();
    virtual DescriptorType type() const override;

    inline VkImage& native_image(size_t index) { return m_images[index % m_nativeCreateInfo.imageCount]; }
    inline VkDeviceMemory& native_memory(size_t index) { return m_memories[index % m_nativeCreateInfo.imageCount]; }
    inline VkImageView& native_image_view(size_t index) { return m_views[index % m_nativeCreateInfo.imageCount]; }

    inline const VkImage& native_image(size_t index) const { return m_images[index % m_nativeCreateInfo.imageCount]; }
    inline const VkDeviceMemory& native_memory(size_t index) const { return m_memories[index % m_nativeCreateInfo.imageCount]; }
    inline const VkImageView& native_image_view(size_t index) const { return m_views[index % m_nativeCreateInfo.imageCount]; }

protected:
    virtual void on_resize() override;

    void copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange, uint32_t index);

private:
    void create();
    void clear();

private:
    VulkanImageCreateInfo m_nativeCreateInfo;
    std::vector<VkImage> m_images;
    std::vector<VkDeviceMemory> m_memories;
    std::vector<VkImageView> m_views;
    bool m_createdFromExternalImage = false;
    DescriptorType m_descriptorType;
};


}

#endif //EAGLE_VULKAN_IMAGE_H
