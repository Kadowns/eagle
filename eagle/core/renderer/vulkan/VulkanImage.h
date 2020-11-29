//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_VULKAN_IMAGE_H
#define EAGLE_VULKAN_IMAGE_H

#include "eagle/core/renderer/Image.h"

#include "VulkanCore.h"
#include <vector>

EG_BEGIN

struct VulkanImageCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
};


class VulkanImage : public Image {

public:

    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo);

    //Used for swapchain images
    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, VkImage image);
    virtual ~VulkanImage();

    inline VkImage& native_image() { return m_image; }
    inline VkDeviceMemory& native_memory() { return m_memory; }
    inline VkImageView& native_image_view() { return m_view; }

    inline const VkImage& native_image() const { return m_image; }
    inline const VkDeviceMemory& native_memory() const { return m_memory; }
    inline const VkImageView& native_image_view() const { return m_view; }

protected:
    virtual void on_resize() override;

    void copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange);

private:
    void create();
    void clear();

private:
    VulkanImageCreateInfo m_nativeCreateInfo;
    VkImage m_image = nullptr;
    VkDeviceMemory m_memory = nullptr;
    VkImageView m_view = nullptr;
    bool m_createdFromExternalImage = false;
};


EG_END

#endif //EAGLE_VULKAN_IMAGE_H
