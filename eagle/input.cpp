//
// Created by Novak on 09/08/2019.
//

#include "input.h"

namespace eagle {

std::unique_ptr<Input> Input::s_instance;

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

bool Input::receive(const OnKey &e) {
    if (e.action == EG_PRESS) {
        m_downKeys.insert(e.key);
        m_pressedKeys.insert(e.key);
    } else if (e.action == EG_RELEASE) {
        m_downKeys.erase(e.key);
        m_releasedKeys.insert(e.key);
    }
    return false;
}

bool Input::receive(const OnMouseMove &e) {
    if (m_firstMouseMove){
        m_mousePosition = {e.x, e.y};
        m_firstMouseMove = false;
    }

    m_mouseDelta = {e.x - m_mousePosition.first, e.y - m_mousePosition.second};

    m_mousePosition = {e.x, e.y};
    return false;
}

bool Input::receive(const OnMouseButton &e) {
    if (e.action == EG_PRESS){
        m_downMouseButtons.insert(e.key);
        m_pressedMouseButtons.insert(e.key);
    }
    else if (e.action == EG_RELEASE){
        m_downMouseButtons.erase(e.key);
        m_releasedMouseButtons.insert(e.key);
    }
    return false;
}

bool Input::receive(const OnMouseScrolled &e) {
    m_scrollDelta = {e.x, e.y};
    return false;
}

void Input::init(EventBus *eventBus) {
    m_listener.attach(eventBus);
    m_listener.subscribe<OnKey>(this);
    m_listener.subscribe<OnMouseMove>(this);
    m_listener.subscribe<OnMouseButton>(this);
    m_listener.subscribe<OnMouseScrolled>(this);
}

void Input::deinit() {
    m_listener.detach();
}

}