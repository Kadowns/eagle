//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_VULKANCONTEXT_H
#define EAGLE_VULKANCONTEXT_H

#include "RenderingContext.h"

struct GLFWwindow;

_EAGLE_BEGIN

class VulkanContext : public RenderingContext {

public:

    VulkanContext(uint32_t width, uint32_t height);

    virtual ~VulkanContext();

    virtual void init(const std::string& windownName)   override;
    virtual void refresh()                              override;
    virtual void deinit()                               override;
    virtual bool should_terminate_context()             override;

private:

    GLFWwindow* m_window = nullptr;

};

_EAGLE_END

#endif //EAGLE_VULKANCONTEXT_H
