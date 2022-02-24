//
// Created by Novak on 29/12/2020.
//

#ifndef EAGLE_EVENTBUS_H
#define EAGLE_EVENTBUS_H

#include <eagle/core_global_definitions.h>

namespace eagle {

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

class ConsumableEventStream {
public:
    using ReturnType = bool;
    typedef std::function<ReturnType(void*)> CallbackType;
private:
    struct Listener {
        size_t id;
        uint32_t priority;
        CallbackType callback;
    };

    struct CopiableMutex : std::mutex {
        CopiableMutex() = default;
        CopiableMutex(CopiableMutex const&) noexcept : std::mutex() {}
        bool operator==(CopiableMutex const&other) noexcept { return this==&other; }
    };
public:

    void emit(void* ev) {
        std::lock_guard<CopiableMutex> lock(m_mutex);
        assert(!m_emitting && "Tried to emmit a event that was already being emitted.");
        m_emitting = true;
        for (auto& listeners : m_listeners) {
            if (listeners.callback(ev)) {
                break;
            }
        }
        m_emitting = false;


        //removes listeners that should be removed
        if (!m_listenersToUnsubscribe.empty()) {
            m_listeners.erase(std::remove_if(m_listeners.begin(), m_listeners.end(), [this](const Listener &listener) {
                if (m_listenersToUnsubscribe.find(listener.id) != m_listenersToUnsubscribe.end()) {
                    m_listenersToUnsubscribe.erase(listener.id);
                    return true;
                }
                return false;
            }), m_listeners.end());
        }
        //adds new listeners
        if (!m_listenersToSubscribe.empty()){
            m_listeners.insert(m_listeners.end(), m_listenersToSubscribe.begin(), m_listenersToSubscribe.end());
            m_listenersToSubscribe.clear();
        }

        sort();
    }

    void subscribe(CallbackType &&callback, size_t listenerId, uint32_t priority) {
        std::lock_guard<CopiableMutex> lock(m_mutex);
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
        std::lock_guard<CopiableMutex> lock(m_mutex);
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
    CopiableMutex m_mutex;
    bool m_emitting = false;
};


template<typename TEventStream>
class GenericEventBus {
public:
    template<typename TEvent>
    void emit(const TEvent& ev){
        emit((void*)&ev, EventHelper::event_index<TEvent>());
    }

    inline void emit(void* ev, size_t eventStreamIndex){
        if (eventStreamIndex >= m_eventStreams.size()){
            return;
        }
        m_eventStreams[eventStreamIndex].emit(ev);
    }

    template<typename TEvent>
    void subscribe(std::function<typename TEventStream::ReturnType(const TEvent&)>&& callback, size_t listenerId, uint32_t priority){
        size_t eventStreamIndex = EventHelper::event_index<TEvent>();

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
        m_eventStreams[eventStreamIndex].unsubscribe(listenerId);
    }

    void unsubscribe_all(size_t listenerId){
        for (auto& stream : m_eventStreams){
            stream.unsubscribe(listenerId);
        }
    }

    size_t next_listener_id() { return m_listenerCount++; }

private:

    std::vector<TEventStream> m_eventStreams;
    size_t m_listenerCount = 0;
};


class BaseImmediateEvent {
public:
    virtual ~BaseImmediateEvent() = default;
    virtual void unsubscribe(size_t listenerId) = 0;
};

template<typename ...Args>
class ImmediateEvent : public BaseImmediateEvent{
public:
    using Callback =  std::function<void(Args&&...)>;
public:

    inline void operator()(Args&&... args){
        emit(std::forward<Args>(args)...);
    }

    void emit(Args&&... args) {
        assert(!m_emitting && "Attempted to emit a event that was already being emitted.");
        m_emitting = true;
        for (auto &callback : m_callbacks) {
            callback(std::forward<Args>(args)...);
        }
        m_emitting = false;
    }

    void subscribe(Callback &&callback, size_t listenerId) {
        assert(!m_emitting && "Attempted to subscribe on a event that is currently emitting");
        assert(std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        }) == m_listeners.end() && "Tried to subscribe to an event with a listener that was already subscribed.");

        m_listeners.emplace_back(listenerId);
        m_callbacks.emplace_back(std::move(callback));
    }

    void unsubscribe(size_t listenerId) override {
        assert(!m_emitting);
        auto it = std::find_if(m_listeners.begin(), m_listeners.end(), [listenerId](size_t listener) {
            return listener == listenerId;
        });

        assert(it != m_listeners.end() && "Attempted to unsubscribe from an event with a listener that is not subscribed.");

        auto callbackIt = std::next(m_callbacks.begin(), std::distance(m_listeners.begin(), it));
        m_listeners.erase(it);
        m_callbacks.erase(callbackIt);
    }

protected:
    std::vector<Callback> m_callbacks;
    std::vector<size_t> m_listeners;
    bool m_emitting = false;
};

class BaseEventListener {

};

template<typename TBus>
class GenericEventListener : private BaseEventListener {
public:

    GenericEventListener() = default;
    GenericEventListener(const GenericEventListener&) = delete;
    GenericEventListener(GenericEventListener&&) = delete;

    virtual ~GenericEventListener() {
        destroy();
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

    void destroy(){
        detach();

        for (auto& event : m_subscribedImmediateEvents){
            event->unsubscribe(m_id);
        }
        m_subscribedImmediateEvents.clear();
    }

    template<typename TEvent, typename TReceiver>
    void receive(TReceiver* receiver, uint32_t priority = 0x7FFFFFFF){
        m_bus->template subscribe<TEvent>([receiver](const TEvent& ev) {
            return receiver->receive(ev);
        }, m_id, priority);
    }

    template<typename TEvent, typename TFunc>
    void subscribe(TFunc&& func, uint32_t priority = 0x7FFFFFFF){
        m_bus->template subscribe<TEvent>(std::forward<TFunc>(func), m_id, priority);
    }

    template<typename TEvent>
    void unsubscribe(){
        m_bus->template unsubscribe<TEvent>(m_id);
    }

    template<typename TEvent>
    void subscribe(TEvent& event, typename TEvent::Callback&& callback){
        event.subscribe(std::move(callback), m_id);
        m_subscribedImmediateEvents.emplace_back(&event);
    }

    template<typename TEvent>
    void unsubscribe(TEvent& event){
        event.unsubscribe(m_id);
        auto it = std::find(m_subscribedImmediateEvents.begin(), m_subscribedImmediateEvents.end(), &event);
        m_subscribedImmediateEvents.erase(it);
    }

private:
    std::vector<BaseImmediateEvent*> m_subscribedImmediateEvents;
    TBus* m_bus = nullptr;
    size_t m_id = 0;
};

using EventBus = GenericEventBus<ConsumableEventStream>;

using EventListener = GenericEventListener<EventBus>;

}

#endif //EAGLE_EVENTBUS_H
