//
// Created by Novak on 09/08/2019.
//

#include "eagle/core/Input.h"

EG_BEGIN

Scope<Input> Input::s_instance;

Input& Input::instance() {
    return s_instance ? *s_instance : *(s_instance = std::make_unique<Input>());
}

bool Input::key_pressed(int key) {
    return m_pressedKeys.find(key) != m_pressedKeys.end();
}

bool Input::key_down(int key) {
    return m_downKeys.find(key) != m_downKeys.end();
}

bool Input::key_released(int key) {
    return m_releasedKeys.find(key) != m_releasedKeys.end();
}

bool Input::mouse_button_down(int button) {
    return m_downMouseButtons.find(button) != m_downMouseButtons.end();;
}

bool Input::mouse_button_pressed(int button) {
    return m_pressedMouseButtons.find(button) != m_pressedMouseButtons.end();;
}

bool Input::mouse_button_released(int button) {
    return m_releasedMouseButtons.find(button) != m_releasedMouseButtons.end();
}

Input::Position Input::mouse_position() {
    return m_mousePosition;
}

Input::Position Input::mouse_move_delta() {
    return m_mouseDelta;
}

Input::Position Input::mouse_scroll_delta() {
    return m_scrollDelta;
}

float Input::mouse_x() {
    return m_mousePosition.first;
}

float Input::mouse_y() {
    return m_mousePosition.second;
}

void Input::refresh() {
    m_releasedKeys.clear();
    m_pressedKeys.clear();
    m_pressedMouseButtons.clear();
    m_releasedMouseButtons.clear();
    m_mouseDelta = {0, 0};
    m_scrollDelta = {0, 0};
}

void Input::handle_key(KeyEvent &e) {
    if (e.get_action() == EG_PRESS){
        m_downKeys.insert(e.get_key());
        m_pressedKeys.insert(e.get_key());
    }
    else if (e.get_action() == EG_RELEASE){
        m_downKeys.erase(e.get_key());
        m_releasedKeys.insert(e.get_key());
    }
}

void Input::handle_mouse_move(MouseMoveEvent &e) {

    if (m_firstMouseMove){
        m_mousePosition = {e.get_x(), e.get_y()};
        m_firstMouseMove = false;
    }

    m_mouseDelta = {e.get_x() - m_mousePosition.first, e.get_y() - m_mousePosition.second};

    m_mousePosition = {e.get_x(), e.get_y()};
}

void Input::handle_mouse_button(MouseButtonEvent &e) {
    if (e.get_action() == EG_PRESS){
        m_downMouseButtons.insert(e.get_key());
        m_pressedMouseButtons.insert(e.get_key());
    }
    else if (e.get_action() == EG_RELEASE){
        m_downMouseButtons.erase(e.get_key());
        m_releasedMouseButtons.insert(e.get_key());
    }
}

void Input::handle_mouse_scroll(MouseScrolledEvent &e) {
    m_scrollDelta = {e.get_x(), e.get_y()};
}



EG_END