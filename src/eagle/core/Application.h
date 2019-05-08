//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_APPLICATION_H
#define EAGLE_APPLICATION_H

#include "Core.h"
#include "Window.h"

_EAGLE_BEGIN

class Application {

public:

    Application(const std::string&  name, int windowWidth, int windowHeight);

    void run();

private:

    Window m_window;

};

_EAGLE_END

#endif //EAGLE_APPLICATION_H
