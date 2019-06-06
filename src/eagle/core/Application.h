//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <memory>
#include "Core.h"
#include "events/Event.h"

_EAGLE_BEGIN

class Window;
class RenderingContext;

struct AppConfig {
    std::string appName;
    Window* windowType;
};

class Application {

public:

    static Application& instance();

    explicit Application(const AppConfig& config);

    void run();
    void handle_event(Event& e);

private:

    static Application* m_instance;

    std::unique_ptr<Window> m_window;

    bool m_shouldClose;

};

_EAGLE_END

#endif //EAGLE_APPLICATION_H
