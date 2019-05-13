//
// Created by Novak on 12/05/2019.
//

#ifndef EAGLE_DEFAULTWINDOW_H
#define EAGLE_DEFAULTWINDOW_H

#include "Core.h"
#include "Window.h"

struct GLFWwindow;

_EAGLE_BEGIN

class DefaultWindow : public Window {
public:

    DefaultWindow(RenderingContext* context);

    virtual void init() override;

    virtual void deinit() override;

    virtual void refresh() override;

    virtual bool should_close() override;

private:

    GLFWwindow* m_window;

};

_EAGLE_END


#endif //EAGLE_DEFAULTWINDOW_H
