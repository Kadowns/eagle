//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_SCENE_H
#define EAGLE_SCENE_H

#include "eagle/engine/EngineCore.h"
#include "Camera.h"

EG_ENGINE_BEGIN

class Scene {
public:

    Scene(const std::string& name);
    ~Scene();

    void update();
    void render(const Reference<Camera> &camera);

    Reference<entityx::EntityManager>& entities() { return m_entities; }
    const Reference<entityx::EntityManager>& entities() const { return m_entities; }

    inline std::string name() { return m_name; }

private:

    std::string m_name;

    entityx::EventManager m_events;
    Reference<entityx::EntityManager> m_entities;
    Reference<RenderingContext> m_renderingContext;
};

EG_ENGINE_END

#endif //EAGLE_SCENE_H
