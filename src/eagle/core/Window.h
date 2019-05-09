//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_WINDOW_H
#define EAGLE_WINDOW_H

#include "Core.h"

#include "../renderer/RenderingContext.h"

_EAGLE_BEGIN

class Window {

public:

    Window(const std::string &name, int width, int height);

    ~Window();

    void init();
    void deinit();
    void refresh();

    bool should_close();
    uint32_t width();
    uint32_t height();

private:

    std::string m_name;

    RenderingContext* m_context;
};

_EAGLE_END

#endif //EAGLE_WINDOW_H
