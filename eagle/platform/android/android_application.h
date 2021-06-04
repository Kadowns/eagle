//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_ANDROIDAPPLICATION_H
#define EAGLE_ANDROIDAPPLICATION_H

#include <eagle/application.h>

struct android_app;
struct AInputEvent;

namespace eagle {

class AndroidWindow;

class AndroidApplication : public Application {
public:
    explicit AndroidApplication(android_app *androidApp, ApplicationDelegate *delegate);

    void run();

    void quit() override { m_quit = true; }
    Window& window() override;
    EventBus& event_bus() override { return m_eventBus; }
    ApplicationDelegate& delegate() override { return *m_delegate; }
protected:

    static void handle_app_cmd(android_app *pApp, int32_t cmd);
    static void handle_input(android_app* app, AInputEvent* event);

protected:
    android_app* m_androidApp;
    StrongPointer<ApplicationDelegate> m_delegate;
    StrongPointer<AndroidWindow> m_window;
    EventBus m_eventBus;

    bool m_quit = false;
};

}

#endif //EAGLE_ANDROIDAPPLICATION_H
