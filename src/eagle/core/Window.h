//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include <string>
#include "Core.h"

struct GLFWwindow;

_EAGLE_BEGIN

class Window {

public:

    Window(const std::string &name, int width, int height);

    ~Window();

    void init();
    void deinit();
    void refresh();

    bool should_close();

private:

    std::string m_name;
    int m_width, m_height;

    GLFWwindow* m_window;

};

_EAGLE_END

#endif //EAGLE_WINDOW_H
