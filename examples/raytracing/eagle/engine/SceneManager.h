//
// Created by Novak on 30/05/2020.
//

#ifndef EAGLE_SCENEMANAGER_H
#define EAGLE_SCENEMANAGER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Scene {
    Scene() : events(), entities(events), systems(entities, events) {}
    entityx::EventManager events;
    entityx::EntityManager entities;
    entityx::SystemManager systems;
};

class SceneManager {
public:
    inline static Scene& current_scene() {
        static SceneManager instance;
        return instance.m_activeScene;
    }
private:
    Scene m_activeScene;
};

EG_ENGINE_END

#endif //EAGLE_SCENEMANAGER_H
