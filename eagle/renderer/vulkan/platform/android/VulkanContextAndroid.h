//
// Created by Ricardo on 2/14/2021.
//

#ifndef EAGLE_VULKANCONTEXTANDROID_H
#define EAGLE_VULKANCONTEXTANDROID_H

#include <eagle/renderer/vulkan/VulkanContext.h>

namespace eagle {

class AndroidWindow;

class VulkanContextAndroid : public VulkanContext {
public:
    VulkanContextAndroid(AndroidWindow* window);
    virtual ~VulkanContextAndroid();

    void init();
    void recreate_surface();
    void destroy_surface();

    inline bool is_initialized() const { return m_initialized; }

protected:
    void create_surface() override;

    std::vector<const char *> get_platform_extensions() override;

protected:
    bool m_initialized = false;
};

}

#endif //EAGLE_VULKANCONTEXTANDROID_H
