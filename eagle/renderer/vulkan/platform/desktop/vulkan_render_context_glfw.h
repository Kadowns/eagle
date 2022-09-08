//
// Created by Ricardo on 2/14/2021.
//

#ifndef EAGLE_VULKANCONTEXTGLFW_H
#define EAGLE_VULKANCONTEXTGLFW_H

#include <eagle/renderer/vulkan/vulkan_render_context.h>

namespace eagle {

class DesktopWindowGLFW;

class VulkanRenderContextGLFW : public VulkanRenderContext {
public:
    VulkanRenderContextGLFW(const RenderContextCreateInfo& createInfo, const VulkanRenderContextCreateInfo& vkCreateInfo);

protected:

    std::vector<const char*> query_instance_extensions() override;

    void create_surface() override;

};

}

#endif //EAGLE_VULKANCONTEXTGLFW_H
