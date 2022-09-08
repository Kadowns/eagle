//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <eagle/core_global_definitions.h>
#include <eagle/log.h>
#include <eagle/events/event.h>

namespace eagle {

class Window;
class Timer;
class ApplicationDelegate;

struct ApplicationCreateInfo {
    const char* applicationName;
    const char* engineName;
    ApplicationDelegate *delegate;
};

class Application {
public:
    explicit Application(const ApplicationCreateInfo&);
    virtual ~Application();
    virtual void quit() = 0;
    virtual Window& window() = 0;
    virtual EventBus& event_bus() = 0;
    virtual ApplicationDelegate& delegate() = 0;

    inline static Application& instance() { return *s_instance; }

protected:
    static Application* s_instance;
    ApplicationCreateInfo m_createInfo;
};

}

#endif //EAGLE_APPLICATION_H
