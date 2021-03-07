//
// Created by Ricardo on 2/14/2021.
//

#ifndef EAGLE_VULKANCONTEXTGLFW_H
#define EAGLE_VULKANCONTEXTGLFW_H

#include <eagle/renderer/vulkan/VulkanContext.h>

namespace eagle {

class DesktopWindowGLFW;

class VulkanContextGLFW : public VulkanContext {
public:
    VulkanContextGLFW(DesktopWindowGLFW* windowGlfw);
    void init();

protected:
    std::vector<const char*> get_platform_extensions() override;

    void create_surface() override;

};

}

#endif //EAGLE_VULKANCONTEXTGLFW_H
