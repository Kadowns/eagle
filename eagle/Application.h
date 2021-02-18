//
// Created by Ricardo on 2/15/2021.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <eagle/CoreGlobalDefinitions.h>
#include <eagle/events/EventBus.h>

EG_BEGIN

class Window;
class Time;
class ApplicationDelegate;

class Application {
public:
    virtual ~Application() = default;
    virtual void quit() = 0;
    virtual Window& window() = 0;
    virtual EventBus& event_bus() = 0;
    virtual ApplicationDelegate& delegate() = 0;

    inline static Application& instance() { return *s_instance; }

protected:
    static Application* s_instance;
};

EG_END

#endif //EAGLE_APPLICATION_H
