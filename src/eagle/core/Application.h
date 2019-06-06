//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <memory>

#include "Core.h"
#include "LayerStack.h"
#include "events/Event.h"
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

    void run();
    void handle_event(Event& e);
    void layer_emplace_back(std::shared_ptr<Layer> layer);
    void layer_emplace_front(std::shared_ptr<Layer> layer);
    void layer_pop(std::shared_ptr<Layer> layer);

private:

    static Application* m_instance;

    std::unique_ptr<Window> m_window;

    bool m_shouldClose = false;

    LayerStack m_layerStack;

};

ApplicationCreateInfo create_application_info();

_EAGLE_END

#endif //EAGLE_APPLICATION_H
