//
// Created by Novak on 22/03/2020.
//

#ifndef EAGLE_IMMEDIATEEVENT_H
#define EAGLE_IMMEDIATEEVENT_H

#include <eagle/core/CoreGlobalDefinitions.h>

EG_BEGIN

template<typename... Args>
class ImmediateEvent {
public:

    using Listener = std::function<void(Args&...)>;

public:

    inline void operator+=(Listener* callback){
        m_listeners.emplace_back(callback);
    }

    inline void operator-=(Listener* callback){
        m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), callback));
    }

    inline void operator()(Args&... args){
        for (auto& f : m_listeners){
            f->operator()(args...);
        }
    }

private:
    std::vector<Listener*> m_listeners;
};

EG_END

#endif //EAGLE_IMMEDIATEEVENT_H
