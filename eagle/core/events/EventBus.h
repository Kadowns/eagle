//
// Created by Novak on 29/12/2020.
//

#ifndef EAGLE_EVENTBUS_H
#define EAGLE_EVENTBUS_H

#include <eagle/core/CoreGlobalDefinitions.h>

EG_BEGIN

class EventHelper {
public:
    template<typename TEvent>
    static size_t event_index(){
        static size_t index = s_eventStreamIndexCounter++;
        return index;
    }
private:
    static size_t s_eventStreamIndexCounter;
};

class EventStream {
public:
    using ReturnType = bool;
    typedef std::function<ReturnType(void*)> CallbackType;
private:
    struct Listener {
        size_t id;
        uint32_t priority;
        CallbackType callback;
    };
public:

    void emit(void* ev) {
        assert(!m_emitting && "Tried to emmit a event that was already being emitted.");
        m_emitting = true;
        for (auto &listeners : m_listeners) {
            if (listeners.callback(ev)) {
                break;
            }
        }
        m_emitting = false;

        //removes listeners that should be removed
        m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(), [this](const Listener& listener){
            if (m_listenersToUnsubscribe.find(listener.id) != m_listenersToUnsubscribe.end()) {
                m_listenersToUnsubscribe.erase(listener.id);
                return true;
            }
            return false;
        }), m_listeners.end());

        //adds new listeners
        m_listeners.insert(m_listeners.end(), m_listenersToSubscribe.begin(), m_listenersToSubscribe.end());
        m_listenersToSubscribe.clear();

        sort();
    }

    void subscribe(CallbackType &&callback, size_t listenerId, uint32_t priority) {
        assert(std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](const Listener& listener) {
            return listener.id == listenerId;
        }) == m_listeners.end() && "Tried to subscribe to an event with a listener that was already subscribed.");
        Listener listener{listenerId, priority, std::move(callback)};
        if (m_emitting){
            m_listenersToSubscribe.emplace_back(std::move(listener));
        }
        else {
            m_listeners.emplace_back(std::move(listener));
            sort();
        }
    }

    void unsubscribe(size_t listenerId) {
        auto it = std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](const Listener& listener) {
            return listener.id == listenerId;
        });

        if (it == m_listeners.end()){
            return;
        }

        if (m_emitting){
            m_listenersToUnsubscribe.insert(listenerId);
        }
        else {
            m_listeners.erase(it);
        }
    }

    inline void sort(){
        std::sort(m_listeners.begin(), m_listeners.end(), [](const Listener& l, const Listener& r){
            return l.priority > r.priority;
        });
    }

protected:
    std::vector<Listener> m_listeners;
    std::vector<Listener> m_listenersToSubscribe;
    std::set<size_t> m_listenersToUnsubscribe;
    bool m_emitting = false;
};


template<typename TEventStream>
class EventBus {
public:
    template<typename TEvent>
    void emit(const TEvent& ev){
        emit((void*)&ev, EventHelper::event_index<TEvent>());
    }

    inline void emit(void* ev, size_t eventStreamIndex){
        if (eventStreamIndex >= m_eventStreams.size()){
            return;
        }
        std::lock_guard lock(m_eventStreamMutex);
        m_eventStreams[eventStreamIndex].emit(&ev);
    }

    template<typename TEvent>
    void subscribe(std::function<typename TEventStream::ReturnType(const TEvent&)>&& callback, size_t listenerId, uint32_t priority){
        size_t eventStreamIndex = EventHelper::event_index<TEvent>();
        std::lock_guard lock(m_eventStreamMutex);
        if (eventStreamIndex >= m_eventStreams.size()){
            m_eventStreams.resize(eventStreamIndex + 1);
        }
        m_eventStreams[eventStreamIndex].subscribe([callback](void* ev){
            return callback(*static_cast<TEvent*>(ev));
        }, listenerId, priority);
    }

    template<typename TEvent>
    void unsubscribe(size_t listenerId){
        size_t eventStreamIndex = EventHelper::event_index<TEvent>();
        if (eventStreamIndex >= m_eventStreams.size()){
            return;
        }
        std::lock_guard lock(m_eventStreamMutex);
        m_eventStreams[eventStreamIndex].unsubscribe(listenerId);
    }

    void unsubscribe_all(size_t listenerId){
        std::lock_guard lock(m_eventStreamMutex);
        for (auto& stream : m_eventStreams){
            stream.unsubscribe(listenerId);
        }
    }

private:

    template<typename TReceiver, typename TBus>
    friend class EventListener;

    size_t next_listener_id(){
        return ++m_listenerIdCounter;
    }

private:

    std::vector<TEventStream> m_eventStreams;
    size_t m_listenerIdCounter = 0;
    std::mutex m_eventStreamMutex;
};

template<typename TReceiver, typename TBus>
class EventListener {
public:

    virtual ~EventListener() {
        detach();
    }

    void attach(TBus* bus){
        if (m_bus){
            detach();
        }
        m_bus = bus;
        m_id = bus->next_listener_id();
    }

    void detach(){
        if (m_bus){
            m_bus->unsubscribe_all(m_id);
        }
        m_bus = nullptr;
    }

    template<typename TEvent>
    void subscribe(TReceiver* receiver, uint32_t priority = 0x7FFFFFFF){
        m_bus->template subscribe<TEvent>([receiver](const TEvent& ev) -> bool{
            return receiver->receive(ev);
        }, m_id, priority);
    }

    template<typename TEvent>
    void unsubscribe(){
        m_bus->template unsubscribe<TEvent>(m_id);
    }

private:
    TBus* m_bus = nullptr;
    size_t m_id = 0;
};

EG_END

#endif //EAGLE_EVENTBUS_H
