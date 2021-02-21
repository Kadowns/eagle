//
// Created by Ricardo on 2/14/2021.
//

#ifndef EAGLE_VULKANCONTEXTGLFW_H
#define EAGLE_VULKANCONTEXTGLFW_H

#include <eagle/renderer/vulkan/VulkanContext.h>

EG_BEGIN

class WindowGLFW;

class VulkanContextGLFW : public VulkanContext {
public:
    VulkanContextGLFW(WindowGLFW* windowGlfw);
    void init(EventBus* eventBus);

protected:
    std::vector<const char*> get_platform_extensions() override;

    void create_surface() override;

};

EG_END

#endif //EAGLE_VULKANCONTEXTGLFW_H
