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
class Time;
class ApplicationDelegate;

class Application {
public:
    Application() { EG_LOG_CREATE("eagle"); }
    virtual ~Application() = default;
    virtual void quit() = 0;
    virtual Window& window() = 0;
    virtual EventBus& event_bus() = 0;
    virtual ApplicationDelegate& delegate() = 0;

    inline static Application& instance() { return *s_instance; }

protected:
    static Application* s_instance;
};

}

#endif //EAGLE_APPLICATION_H
