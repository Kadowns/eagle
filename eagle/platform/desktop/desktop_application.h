//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_DESKTOPAPPLICATION_H
#define EAGLE_DESKTOPAPPLICATION_H

#include "eagle/core_global_definitions.h"
#include "eagle/application.h"
#include "eagle/log.h"
#include "eagle/events/event.h"
#include "eagle/memory/pointer.h"

namespace eagle {

class DesktopWindowGLFW;
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
    StrongPointer<ApplicationDelegate> m_delegate;
    StrongPointer<DesktopWindowGLFW> m_window;
    EventBus m_eventBus;

    bool m_quit = false;
};

}

#endif //EAGLE_DESKTOPAPPLICATION_H
