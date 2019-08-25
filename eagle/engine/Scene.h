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

    Scene();
    ~Scene();

    void init();
    void update();
    void render(const Reference<Camera> &camera);

    Reference<entityx::EntityManager>& entities() { return m_entities; }
    const Reference<entityx::EntityManager>& entities() const { return m_entities; }

private:

    struct {
        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;
    } sceneViewport;

    entityx::EventManager m_events;
    Reference<entityx::EntityManager> m_entities;
    Reference<RenderingContext> m_renderingContext;

    Handle<Shader> m_shader;
    Handle<DescriptorSetLayout> m_materialDescriptorSetLayout;
};

EG_ENGINE_END

#endif //EAGLE_SCENE_H
