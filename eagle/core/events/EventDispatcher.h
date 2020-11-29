//
// Created by Novak on 29/03/2020.
//

#ifndef EAGLE_EVENTDISPATCHER_H
#define EAGLE_EVENTDISPATCHER_H

#include <eagle/core/CoreGlobalDefinitions.h>

EG_BEGIN

class EventDispatcher {
private:

    class BaseEvent {
    public:
        virtual void casted_invoke(const std::any& data) = 0;
    };


    template<typename T>
    class Event : public BaseEvent {
    public:
        virtual void casted_invoke(const std::any& data) override {
            for (auto& listener : m_listeners){
                listener->operator()(std::any_cast<const T&>(data));
            }
        }

        void add_listener(std::function<void(const T&)>* listener){
            m_listeners.emplace_back(listener);
        }

        void remove_listener(std::function<void(const T&)>* listener){
            m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener));
        }

    private:
        std::vector<std::function<void(const T&)>*> m_listeners;
    };


public:

    template<typename TEvent>
    void emit(const TEvent& event){
        static std::type_index index(typeid(TEvent));
        if (m_listeners.find(index) == m_listeners.end()){
            return;
        }
        m_listeners[index]->casted_invoke(event);
    }

    template<typename TEvent>
    void subscribe(std::function<void(const TEvent&)>* listener){
        static std::type_index index(typeid(TEvent));
        if (m_listeners.find(index) == m_listeners.end()){
            m_listeners.emplace(index, std::make_unique<Event<TEvent>>());
        }
        auto castedEvent = std::static_pointer_cast<Event<TEvent>>(m_listeners.at(index));
        castedEvent->add_listener(listener);
    }

    template<typename TEvent>
    void unsubscribe(std::function<void(const TEvent&)>* listener){
        static std::type_index index(typeid(TEvent));
        if (m_listeners.find(index) == m_listeners.end()){
            return;
        }
        auto castedEvent = std::static_pointer_cast<Event<TEvent>>(m_listeners.at(index));
        castedEvent->remove_listener(listener);
    }

private:
    std::unordered_map<std::type_index, Reference<BaseEvent>> m_listeners;
};

EG_END

#endif //EAGLE_EVENTDISPATCHER_H
