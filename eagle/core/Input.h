//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_INPUT_H
#define EAGLE_INPUT_H

#include <memory>

#include "Core.h"

_EAGLE_BEGIN

class Input {

public:

    virtual ~Input() = default;

    static inline bool key_pressed(int key) {return s_instance->handle_key_pressed(key);}
    static inline bool key_down(int key) {return s_instance->handle_key_down(key);};
    static inline bool key_released(int key) {return s_instance->handle_key_released(key);};
    static inline bool mouse_button_down(int button) {return s_instance->handle_mouse_button_down(button);};
    static inline bool mouse_button_pressed(int button) {return s_instance->handle_mouse_button_pressed(button);};
    static inline bool mouse_button_released(int button) {return s_instance->handle_mouse_button_released(button);};
    static inline std::pair<float, float> mouse_position() {return s_instance->handle_mouse_position();};
    static inline float mouse_x() {return s_instance->handle_mouse_x();};
    static inline float mouse_y() {return s_instance->handle_mouse_y();};

protected:

    static std::unique_ptr<Input> s_instance;

    virtual bool handle_key_down(int keycode) = 0;
    virtual bool handle_key_pressed(int keycode) = 0;
    virtual bool handle_key_released(int keycode) = 0;

    virtual bool handle_mouse_button_down(int button) = 0;
    virtual bool handle_mouse_button_pressed(int button) = 0;
    virtual bool handle_mouse_button_released(int button) = 0;
    virtual std::pair<float, float> handle_mouse_position() = 0;
    virtual float handle_mouse_x() = 0;
    virtual float handle_mouse_y() = 0;

};


_EAGLE_END

#endif //EAGLE_INPUT_H
