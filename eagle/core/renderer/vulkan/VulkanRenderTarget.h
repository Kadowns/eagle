//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_VULKANRENDERTARGET_H
#define EAGLE_VULKANRENDERTARGET_H

#include <vector>

#include "eagle/core/renderer/RenderTarget.h"

#include "VulkanCore.h"
#include "VulkanImage.h"

EG_BEGIN

struct VulkanRenderTargetCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkFormat colorFormat, depthFormat;
    VkRenderPass renderPass;
};

class VulkanRenderTarget : public RenderTarget{
public:
    explicit VulkanRenderTarget(uint32_t width, uint32_t height,
                                const std::vector<RENDER_TARGET_ATTACHMENT> &attachments,
                                VulkanRenderTargetCreateInfo &info);
    ~VulkanRenderTarget();

    void create_resources();
    void create_framebuffer();

    void cleanup();
    void create(uint32_t width, uint32_t height);
    VkExtent2D& get_extent() {return m_extent;}

    virtual Handle<Image> get_image(size_t index) override;
    virtual std::vector<Handle<Image>> get_images() override;

    VkFramebuffer& get_framebuffer() {return m_framebuffer;}
    //VkRenderPass & get_render_pass() {return m_renderPass;}
    std::vector<VkClearValue> get_clear_values();

private:

    void create_image_resources(Reference<VulkanImageAttachment> &image, VkFormat format,
                                VkImageUsageFlags usageFlags, VkImageAspectFlags aspectMask);

private:

    VulkanRenderTargetCreateInfo m_info;
    Reference<VulkanImageAttachment> m_depthAttachment;
    std::vector<Reference<VulkanImage>> m_images;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;

    bool m_cleared = true;

};

EG_END

#endif //EAGLE_VULKANRENDERTARGET_H
