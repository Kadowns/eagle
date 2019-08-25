//
// Created by Novak on 05/06/2019.
// Code HEAVILY based on Hazel event system: https://github.com/TheCherno/Hazel
//

#ifndef EAGLE_WINDOWEVENTS_H
#define EAGLE_WINDOWEVENTS_H

#include "Event.h"

EG_BEGIN

class WindowResizedEvent : public Event {

public:

    WindowResizedEvent(uint32_t width, uint32_t height) :
        m_width(width), m_height(height){}

    inline uint32_t get_width() const {return m_width;}
    inline uint32_t get_height() const {return m_height;}

private:
    uint32_t m_width, m_height;

};

class WindowCloseEvent : public Event {
public:

};

class WindowFocusEvent : public Event{
public:

};

class WindowLostFocusEvent : public Event{
public:

};

class WindowMoveEvent : public Event{
public:

};

EG_END

#endif //EAGLE_WINDOWEVENTS_H
