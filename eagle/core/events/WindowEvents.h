//
// Created by Novak on 05/06/2019.
// Code HEAVILY based on Hazel event system: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_WINDOWEVENTS_H
#define EAGLE_WINDOWEVENTS_H

#include "Event.h"

_EAGLE_BEGIN

class WindowResizedEvent : public Event {

public:
    EVENT_CLASS_TYPE(EVENT_TYPE::WINDOW_RESIZE)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW)

    WindowResizedEvent(uint32_t width, uint32_t height) :
        m_width(width), m_height(height){}

    inline uint32_t get_width() const {return m_width;}
    inline uint32_t get_height() const {return m_height;}

private:
    uint32_t m_width, m_height;

};

class WindowCloseEvent : public Event {
public:
    EVENT_CLASS_TYPE(EVENT_TYPE::WINDOW_CLOSE)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW)
};

class WindowFocusEvent : public Event{
public:
    EVENT_CLASS_TYPE(EVENT_TYPE::WINDOW_FOCUS)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW)
};

class WindowLostFocusEvent : public Event{
public:
    EVENT_CLASS_TYPE(EVENT_TYPE::WINDOW_LOST_FOCUS)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW)
};

class WindowMoveEvent : public Event{
public:
    EVENT_CLASS_TYPE(EVENT_TYPE::WINDOW_MOVED)
    EVENT_CLASS_CATEGORY(EVENT_CATEGORY_WINDOW)
};

_EAGLE_END

#endif //EAGLE_WINDOWEVENTS_H
