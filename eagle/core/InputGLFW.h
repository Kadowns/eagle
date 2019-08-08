//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_INPUTGLFW_H
#define EAGLE_INPUTGLFW_H

#include "Core.h"
#include "Input.h"

struct GLFWwindow;

_EAGLE_BEGIN

class InputGLFW : public Input {
public:

    InputGLFW(GLFWwindow* window);
    virtual ~InputGLFW();

protected:
    virtual bool handle_key_down(int keycode) override final;
    virtual bool handle_key_pressed(int keycode) override final;
    virtual bool handle_key_released(int keycode) override final;
    virtual bool handle_mouse_button_down(int button) override final;
    virtual bool handle_mouse_button_pressed(int button) override final;
    virtual bool handle_mouse_button_released(int button) override final;
    virtual std::pair<float, float> handle_mouse_position() override final;
    virtual float handle_mouse_x() override final;
    virtual float handle_mouse_y() override final;
private:
    GLFWwindow* m_window;

};

_EAGLE_END

#endif //EAGLE_INPUTGLFW_H
