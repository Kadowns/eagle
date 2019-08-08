#include <utility>

//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_EVENT_H
#define EAGLE_EVENT_H

#include <functional>
#include <map>
#include <typeindex>

#include "eagle/core/Core.h"

_EAGLE_BEGIN

class EventDispatcher;

class Event {
public:
    Event() = default;
    virtual ~Event() = default;

    virtual std::type_index get_type_index( ) const {
        return std::type_index( typeid(*this) );
    }

    bool is_handled() { return m_handled; }
private:
    friend class EventDispatcher;
    bool m_handled = false;
};


class EventDispatcher {
    using EventFunctionHandle = std::function<bool(Eagle::Event&)>;
public:
    EventDispatcher() = default;

    template<typename T>
    void add_listener(EventFunctionHandle func){
        static_assert(std::is_base_of<Eagle::Event, T>(), "Template argument does not inherit from Eagle::Event");
        std::type_index index(typeid(T));
        if (m_listeners.count(index) == 0){
            m_listeners[index] = std::move(func);
        }
    }

    bool dispatch(Eagle::Event& e) {
        auto listener = m_listeners.find(e.get_type_index());
        if (listener != m_listeners.end()){
            e.m_handled = listener->second.operator()(e);
            return true;
        }
        return false;
    }

private:
    std::map<std::type_index, EventFunctionHandle> m_listeners;
};

_EAGLE_END

#endif //EAGLE_EVENT_H
