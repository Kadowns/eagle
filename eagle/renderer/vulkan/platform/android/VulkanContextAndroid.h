//
// Created by Ricardo on 2/14/2021.
//

#ifndef EAGLE_VULKANCONTEXTANDROID_H
#define EAGLE_VULKANCONTEXTANDROID_H

#include <eagle/renderer/vulkan/VulkanContext.h>

EG_BEGIN

class AndroidWindow;

class VulkanContextAndroid : public VulkanContext {
public:
    VulkanContextAndroid(AndroidWindow* window);
    virtual ~VulkanContextAndroid();

    void on_surface_created();
    void on_surface_destroyed();

protected:
    void create_surface() override;

    std::vector<const char *> get_platform_extensions() override;

protected:

};

EG_END

#endif //EAGLE_VULKANCONTEXTANDROID_H
