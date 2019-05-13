//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include <memory>
#include "Core.h"

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

    Application(const AppConfig& config);

    void run();

private:

    static Application* m_instance;

    Window* m_window;

};

_EAGLE_END

#endif //EAGLE_APPLICATION_H
