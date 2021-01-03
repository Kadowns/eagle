//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H



#include "CoreGlobalDefinitions.h"
#include "LayerStack.h"
#include "Log.h"
#include "events/WindowEvents.h"
#include "Input.h"


EG_BEGIN

class Window;
class RenderingContext;

struct ApplicationCreateInfo {
    std::string appName;
    Window* windowType;
    std::vector<Reference<Layer>> layers;
    Log::LOG_LEVEL clientLogLevel, coreLogLevel;
};

class Application {

public:

    static Application& instance();

    explicit Application(const ApplicationCreateInfo& config);
     ~Application() = default;

    void run();
    void quit();

    inline LayerStack& layer_stack() { return m_layerStack; }
    inline Window& window() { return *m_window; }
    inline EventBus& event_bus() { return m_eventBus; }

private:
    static Application* m_instance;

    Reference<Window> m_window;
    LayerStack m_layerStack;
    EventBus m_eventBus;

    bool m_quit = false;
};

EG_END

#endif //EAGLE_APPLICATION_H
