//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_INPUTEVENTS_H
#define EAGLE_INPUTEVENTS_H

#include "Event.h"

EG_BEGIN

class MouseMoveEvent : public Event {

public:

    MouseMoveEvent(double x, double y)
        : m_x(x), m_y(y){}

    double get_x() const {return m_x;}
    double get_y() const {return m_y;}

private:

    double m_x, m_y;

};

class MouseButtonEvent : public Event {
public:

    MouseButtonEvent(int key, int action, int mods) :  m_key(key), m_action(action), m_mods(mods){}

    int get_key() const { return m_key; }
    int get_action() const { return m_action; }
    int get_mods() const { return m_mods; }

private:
    int m_key, m_action, m_mods;
};

class MouseScrolledEvent : public Event {
public:

    MouseScrolledEvent(float x, float y) :  m_x(x), m_y(y) {}

    inline float get_x() const {return m_x;}
    inline float get_y() const {return m_y;}

private:
    float m_x, m_y;
};

class KeyEvent : public Event {

public:

    KeyEvent(int key, int action, int mods) : m_key(key), m_action(action), m_mods(mods) {}

    inline int get_key()  const {return m_key;}
    inline int get_mods() const {return m_mods;}
    inline int get_action() const{return m_action;}

private:
    int m_key, m_action, m_mods;

};

class KeyTypedEvent : public Event {

public:

    KeyTypedEvent(uint32_t key) : m_key(key) {}

    inline uint32_t get_key()  const {return m_key;}

private:
    uint32_t m_key;

};

EG_END

#endif //EAGLE_INPUTEVENTS_H
