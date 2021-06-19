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

class VulkanImage;


struct VulkanImageViewCreateInfo {
    VkDevice device;
    uint32_t imageCount;
    VkImageViewType viewType;
    VkImageSubresourceRange subresourceRange;
    VkFormat format;
    DescriptorType descriptorType;
    VulkanImage* image;
};

class VulkanImageView : public ImageView {
public:
    VulkanImageView(const VulkanImageViewCreateInfo& createInfo);
    DescriptorType type() const override;

    ~VulkanImageView() override;

    uint32_t mip_level() const override;

    Image& image() const override;

    inline VkImageView& native_image_view(size_t index) { return m_views[index % m_createInfo.imageCount]; }
    inline const VkImageView& native_image_view(size_t index) const { return m_views[index % m_createInfo.imageCount]; }

private:
    VulkanImageViewCreateInfo m_createInfo;
    std::vector<VkImageView> m_views;
};

class VulkanImage : public Image {
public:
    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo);

    //Used for swapchain images
    VulkanImage(const ImageCreateInfo& imageCreateInfo, const VulkanImageCreateInfo& nativeCreateInfo, std::vector<VkImage> images);
    ~VulkanImage();

    void generate_mipmaps() override;
    WeakPointer<ImageView> view(uint32_t mipLevel = 0) override;

    inline VkImage& native_image(size_t index) { return m_images[index % m_nativeCreateInfo.imageCount]; }
    inline VkDeviceMemory& native_memory(size_t index) { return m_memories[index % m_nativeCreateInfo.imageCount]; }
    inline WeakPointer<VulkanImageView> native_view(uint32_t mipLevel = 0) { return m_views[mipLevel]; }

    inline const VkImage& native_image(size_t index) const { return m_images[index % m_nativeCreateInfo.imageCount]; }
    inline const VkDeviceMemory& native_memory(size_t index) const { return m_memories[index % m_nativeCreateInfo.imageCount]; }
    inline const WeakPointer<VulkanImageView> native_view(uint32_t mipLevel = 0) const { return m_views[mipLevel]; }

protected:
    void on_resize() override;

    void copy_buffer_data_to_image(VkImageSubresourceRange subresourceRange, uint32_t index);

private:
    void create();
    void clear();

private:
    VulkanImageCreateInfo m_nativeCreateInfo;
    std::vector<VkImage> m_images;
    std::vector<VkDeviceMemory> m_memories;
    std::vector<StrongPointer<VulkanImageView>> m_views;
    bool m_createdFromExternalImage = false;
};


}

#endif //EAGLE_VULKAN_IMAGE_H
