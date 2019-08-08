//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_VULKANRENDERTARGET_H
#define EAGLE_VULKANRENDERTARGET_H

#include <memory>
#include <vector>

#include "eagle/renderer/RenderTarget.h"

#include "VulkanCore.h"
#include "VulkanImage.h"

_EAGLE_BEGIN

struct VulkanRenderTargetCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkFormat colorFormat;
    VkFormat depthFormat;
};

class VulkanRenderTarget : public RenderTarget{
public:
    explicit VulkanRenderTarget(uint32_t width, uint32_t height,
                                const std::vector<RENDER_TARGET_ATTACHMENT> &attachments,
                                VulkanRenderTargetCreateInfo &info);
    ~VulkanRenderTarget();

    void create_resources();
    void create_render_pass();
    void create_framebuffer();

    void cleanup();
    void create(uint32_t width, uint32_t height);
    VkExtent2D& get_extent() {return m_extent;}

    virtual std::weak_ptr<Image> get_image(size_t index) override;
    virtual std::vector<std::weak_ptr<Image>> get_images() override;

    VkFramebuffer& get_framebuffer() {return m_framebuffer;}
    VkRenderPass & get_render_pass() {return m_renderPass;}
    std::vector<VkClearValue> get_clear_values();

private:

    void create_image_resources(std::shared_ptr<VulkanImageAttachment> &image, VkFormat format,
                                VkImageUsageFlags usageFlags, VkImageAspectFlags aspectMask);

private:

    VulkanRenderTargetCreateInfo m_info;
    std::shared_ptr<VulkanImageAttachment> m_depthAttachment;
    std::vector<std::shared_ptr<VulkanImage>> m_images;
    VkFramebuffer m_framebuffer;
    VkRenderPass m_renderPass;
    VkExtent2D m_extent;

    bool m_cleared = true;

};

_EAGLE_END

#endif //EAGLE_VULKANRENDERTARGET_H
