//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_DESKTOPAPPLICATION_H
#define EAGLE_DESKTOPAPPLICATION_H

#include "eagle/CoreGlobalDefinitions.h"
#include "eagle/Application.h"
#include "eagle/Log.h"
#include "eagle/events/EventBus.h"

EG_BEGIN

class WindowGLFW;
class RenderingContext;

class DesktopApplication : public Application {
public:

     DesktopApplication(uint32_t width, uint32_t height, ApplicationDelegate *delegate);
     ~DesktopApplication() = default;

    void run();

    void quit() override { m_quit = true; }
    Window& window() override;
    EventBus& event_bus() override { return m_eventBus; }
    ApplicationDelegate& delegate() override { return *m_delegate; }

protected:
    Reference<ApplicationDelegate> m_delegate;
    Reference<WindowGLFW> m_window;
    EventBus m_eventBus;

    bool m_quit = false;
};

EG_END

#endif //EAGLE_DESKTOPAPPLICATION_H
