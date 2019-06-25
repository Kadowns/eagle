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
    explicit VulkanRenderTarget(uint32_t width, uint32_t height, VulkanRenderTargetCreateInfo &info);
    ~VulkanRenderTarget();

    void create_resources();
    void create_render_pass();
    void create_framebuffer();

    void cleanup();
    void create(uint32_t width, uint32_t height);

    virtual std::weak_ptr<Image> get_image() override {return m_color;}
    virtual uint32_t get_width() override {return m_extent.width;}
    virtual uint32_t get_height() override {return m_extent.height;}
    VkExtent2D& get_extent() {return m_extent;}

    std::weak_ptr<Image> get_depth_image() {return m_depth;}
    VkFramebuffer& get_framebuffer() {return m_framebuffer;}
    VkRenderPass & get_render_pass() {return m_renderPass;}

private:

    VulkanRenderTargetCreateInfo m_info;
    std::shared_ptr<VulkanImage> m_color, m_depth;
    VkFramebuffer m_framebuffer;
    VkRenderPass m_renderPass;
    VkExtent2D m_extent;

    bool m_cleared = true;

};

_EAGLE_END

#endif //EAGLE_VULKANRENDERTARGET_H
