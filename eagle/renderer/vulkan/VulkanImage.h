//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_VULKAN_IMAGE_H
#define EAGLE_VULKAN_IMAGE_H

#include "eagle/renderer/Image.h"

#include "VulkanCore.h"
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

    inline std::vector<VkImage>& native_images() { return m_images; }
    inline std::vector<VkDeviceMemory>& native_memories() { return m_memories; }
    inline std::vector<VkImageView>& native_image_views() { return m_views; }

    inline const std::vector<VkImage>& native_images() const  { return m_images; }
    inline const std::vector<VkDeviceMemory>& native_memories() const  { return m_memories; }
    inline const std::vector<VkImageView>& native_image_views() const  { return m_views; }

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
