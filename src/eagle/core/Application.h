//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <memory>
#include <queue>

#include "Core.h"
#include "LayerStack.h"
#include "events/Event.h"
#include "events/WindowEvents.h"
#include "Log.h"



_EAGLE_BEGIN

class Window;
class RenderingContext;

struct ApplicationCreateInfo {
    std::string appName;
    Window* windowType;
    std::vector<std::shared_ptr<Layer>> layers;
    Log::LOG_LEVEL clientLogLevel, coreLogLevel;
};

class Application {

public:

    static Application& instance();

    explicit Application(const ApplicationCreateInfo& config);
     ~Application() = default;

    void run();
    void handle_event(std::shared_ptr<Event> e);
    void dispatch_events();
    void event_emplace_back(std::shared_ptr<Event> e);
    void layer_emplace_back(std::shared_ptr<Layer> layer);
    void layer_emplace_front(std::shared_ptr<Layer> layer);
    void layer_emplace(std::vector<std::shared_ptr<Layer>> layers);
    void layer_pop(std::shared_ptr<Layer> layer);

    static float get_window_aspect();
    static uint32_t get_window_width();
    static uint32_t get_window_height();

private:

    using EventQueue = std::queue<std::shared_ptr<Event>>;

    bool window_close_event(WindowCloseEvent& e);

    static Application* m_instance;

    std::unique_ptr<Window> m_window;

    bool m_shouldClose = false;

    LayerStack m_layerStack;
    EventQueue m_eventQueue;

};

_EAGLE_END

#endif //EAGLE_APPLICATION_H
