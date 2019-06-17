//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_INPUTEVENTS_H
#define EAGLE_INPUTEVENTS_H

#include "eagle/core/Core.h"
#include "Event.h"

_EAGLE_BEGIN

class MouseMoveEvent : public Event {

public:

    EVENT_CLASS_TYPE(EVENT_TYPE::MOUSE_MOVE)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE)

    MouseMoveEvent(float x, float y)
        : m_x(x), m_y(y){}

    float get_x() const {return m_x;}
    float get_y() const {return m_y;}

private:

    float m_x, m_y;

};

class MouseClickEvent : public Event {

public:
    EVENT_CLASS_TYPE(EVENT_TYPE::MOUSE_BUTTON_PRESSED)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_INPUT | EVENT_CATEGORY_MOUSE)

    MouseClickEvent(float x, float y, int key) :
        m_x(x), m_y(y), m_key(key){}

    float get_x() const {return m_x;}
    float get_y() const {return m_y;}
    int get_key() const {return m_key;}

private:
    float m_x, m_y;
    int m_key;
};


_EAGLE_END

#endif //EAGLE_INPUTEVENTS_H
