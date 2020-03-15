//
// Created by Novak on 10/11/2019.
//

#include <eagle/core/InputLayer.h>
#include <eagle/core/events/WindowEvents.h>
#include <eagle/core/events/InputEvents.h>
#include <eagle/core/Input.h>

EG_BEGIN

void InputLayer::handle_attach() {

    m_dispatcher.add_listener<MouseMoveEvent>([&](Event& e){
        Input::instance().handle_mouse_move(*(MouseMoveEvent*)&e);
        return false;
    });

    m_dispatcher.add_listener<MouseButtonEvent>([&](Event& e){
        Input::instance().handle_mouse_button(*(MouseButtonEvent*)&e);
        return false;
    });

    m_dispatcher.add_listener<MouseScrolledEvent>([&](Event& e){
        Input::instance().handle_mouse_scroll(*(MouseScrolledEvent*)&e);
        return false;
    });

    m_dispatcher.add_listener<KeyEvent>([&](Event& e){
        Input::instance().handle_key(*(KeyEvent*)&e);
        return false;
    });
}

void InputLayer::handle_deattach() {

}

void InputLayer::handle_update() {
    Input::instance().refresh();
}

void InputLayer::handle_event(Event &e) {
    m_dispatcher.dispatch(e);
}


EG_END

