//
// Created by Novak on 29/03/2020.
//

#ifndef EAGLE_EVENTMASTER_H
#define EAGLE_EVENTMASTER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/core/events/EventDispatcher.h>

EG_ENGINE_BEGIN

struct OnRaytracerTargetCreated {
    OnRaytracerTargetCreated(const Reference<Image> target) : target(target) {}
    Reference<Image> target;
};

struct OnSceneRecreate {};

class EventMaster {
public:
    static EventDispatcher& instance(){
        static EventDispatcher s_instance;
        return s_instance;
    }
};

EG_ENGINE_END

#endif //EAGLE_EVENTMASTER_H
