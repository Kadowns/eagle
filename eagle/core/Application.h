//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <memory>
#include <queue>

#include "GlobalDefinitions.h"
#include "LayerStack.h"
#include "Log.h"
#include "events/Event.h"
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
    void handle_event(Reference<Event> e);
    void dispatch_events();
    void quit();
    void event_emplace_back(Reference<Event> e);
    void layer_emplace_back(Reference<Layer> layer);
    void layer_emplace_front(Reference<Layer> layer);
    void layer_emplace(std::vector<Reference<Layer>> layers);
    void layer_pop(Reference<Layer> layer);


    inline Window& window() { return *m_window; }

private:

    using EventQueue = std::queue<Reference<Event>>;

    static Application* m_instance;

    Reference<Window> m_window;

    bool m_quit = false;

    LayerStack m_layerStack;
    EventQueue m_eventQueue;


};

EG_END

#endif //EAGLE_APPLICATION_H
