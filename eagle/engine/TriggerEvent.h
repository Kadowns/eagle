//
// Created by Novak on 31/08/2019.
//

#ifndef EAGLE_TRIGGEREVENT_H
#define EAGLE_TRIGGEREVENT_H

#include "GlobalDefs.h"

EG_ENGINE_BEGIN

template<typename... Args>
class TriggerEvent {
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

EG_ENGINE_END

#endif //EAGLE_TRIGGEREVENT_H
