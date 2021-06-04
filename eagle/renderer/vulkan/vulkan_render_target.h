//
// Created by Novak on 23/06/2019.
//

#ifndef EAGLE_VULKANRENDERTARGET_H
#define EAGLE_VULKANRENDERTARGET_H

#include <vector>

#include "eagle/renderer/render_target.h"

#include "vulkan_global_definitions.h"
#include "vulkan_image.h"

namespace eagle {

struct VulkanRenderTargetCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkFormat colorFormat, depthFormat;
};

class VulkanRenderTarget : public RenderTarget {
public:
    virtual VkRenderPass& get_render_pass() = 0;
    virtual VkFramebuffer& get_framebuffer() = 0;
    virtual VkExtent2D& get_extent() = 0;
    virtual std::vector<VkClearValue> get_clear_values() = 0;
};


class VulkanCustomRenderTarget : public VulkanRenderTarget {
public:
    explicit VulkanCustomRenderTarget(uint32_t width, uint32_t height, VkRenderPass &renderPass,
                                      VulkanRenderTargetCreateInfo &info);
    ~VulkanCustomRenderTarget();

    void create_resources();
    void create_framebuffer();
    void cleanup();
    void create(uint32_t width, uint32_t height);

    virtual WeakPointer<Image> get_image() override;
    virtual VkExtent2D& get_extent() override {return m_extent;}
    virtual VkRenderPass& get_render_pass() override {return m_renderPass;}
    virtual VkFramebuffer& get_framebuffer() override {return m_framebuffer;}
    virtual std::vector<VkClearValue> get_clear_values() override;

private:

    void create_image_resources(std::shared_ptr<VulkanImageAttachment> &image, VkFormat format,
                                VkImageUsageFlags usageFlags, VkImageAspectFlags aspectMask);

private:

    VulkanRenderTargetCreateInfo m_info;
    VkRenderPass& m_renderPass;
    std::shared_ptr<VulkanImageAttachment> m_depthAttachment;
    std::shared_ptr<VulkanImage> m_image;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;

    bool m_cleared = true;
};


class VulkanMainRenderTarget : public VulkanRenderTarget {

public:
    VulkanMainRenderTarget(const VulkanRenderTargetCreateInfo &info, VkImage image, VkRenderPass& renderPass, uint32_t width, uint32_t height);

    virtual ~VulkanMainRenderTarget() override;
    virtual WeakPointer<Image> get_image() override;
    virtual VkRenderPass &get_render_pass() override;
    virtual VkFramebuffer &get_framebuffer() override;
    virtual VkExtent2D &get_extent() override;
    virtual std::vector<VkClearValue> get_clear_values() override;

private:
    VulkanRenderTargetCreateInfo m_info;
    VkRenderPass& m_renderPass;
    std::shared_ptr<VulkanImage> m_image;
    std::shared_ptr<VulkanImageAttachment> m_depthAttachment;
    VkFramebuffer m_framebuffer;
    VkExtent2D m_extent;


};

}

#endif //EAGLE_VULKANRENDERTARGET_H
