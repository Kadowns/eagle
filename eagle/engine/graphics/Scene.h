//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_SCENE_H
#define EAGLE_SCENE_H

#include "eagle/engine/EngineCore.h"
#include "eagle/engine/graphics/Camera.h"

EG_ENGINE_BEGIN

class Scene {
public:

    Scene();
    ~Scene();

    void init(const std::shared_ptr<Camera> &camera);
    void update();
    void render(const std::shared_ptr<Camera> &camera);

    std::shared_ptr<entityx::EntityManager>& entities() { return m_entities; }

private:

    struct {
        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;
    } sceneViewport;

    entityx::EventManager m_events;
    std::shared_ptr<entityx::EntityManager> m_entities;
    std::shared_ptr<RenderingContext> m_renderingContext;

    std::weak_ptr<Shader> m_shader;
    std::weak_ptr<DescriptorSetLayout> m_materialDescriptorSetLayout;
};

EG_ENGINE_END

#endif //EAGLE_SCENE_H
