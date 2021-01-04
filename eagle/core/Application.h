//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H



#include "CoreGlobalDefinitions.h"
#include "LayerStack.h"
#include "Log.h"
#include "Time.h"


EG_BEGIN

class Window;
class RenderingContext;

class Application {

public:

    static Application& instance();

    explicit Application(const std::string &appName, Reference<Window> window);
     ~Application() = default;

    void run();
    void quit();

    inline LayerStack& layer_stack() { return m_layerStack; }
    inline Window& window() { return *m_window; }
    inline EventBus& event_bus() { return m_eventBus; }

protected:
    static Application* m_instance;

    Reference<Window> m_window;
    LayerStack m_layerStack;
    EventBus m_eventBus;

    bool m_quit = false;
};

EG_END

#endif //EAGLE_APPLICATION_H
