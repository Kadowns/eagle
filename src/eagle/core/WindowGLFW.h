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
    virtual void begin_draw() override;
    virtual void end_draw() override;
    virtual void refresh() override;

    virtual void handle_events() override;
    virtual void set_event_callback(PFN_EventCallback callback) override;

    virtual void* get_native_window() override;

    virtual bool is_minimized() override;
    virtual void wait_native_events() override;

private:

    GLFWwindow* m_window;
};

_EAGLE_END


#endif //EAGLE_WINDOWGLFW_H
