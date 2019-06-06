//
// Created by Novak on 12/05/2019.
//

#ifndef EAGLE_WINDOWGLFW_H
#define EAGLE_WINDOWGLFW_H


#include "Core.h"
#include "Window.h"

struct GLFWwindow;

_EAGLE_BEGIN

class WindowGLFW : public Window {
public:

    WindowGLFW(RenderingContext* context, uint32_t width, uint32_t height);

    virtual void init() override;
    virtual void deinit() override;
    virtual void refresh() override;

    virtual void handle_events() override;

    virtual void* get_native_window() override;

    virtual bool is_minimized() override;
    virtual void wait_native_events() override;

    virtual size_t add_event_listener(PFN_EventCallback callback) override;
    virtual void remove_event_listener(size_t identifier) override;

private:

    GLFWwindow* m_window;
};

_EAGLE_END


#endif //EAGLE_WINDOWGLFW_H
