//
// Created by Novak on 12/06/2019.
//

#ifndef EAGLE_SCENELAYER_H
#define EAGLE_SCENELAYER_H

#include <entityx/entityx.h>

#include "EngineCore.h"
#include "graphics/Scene.h"

EG_ENGINE_BEGIN

class SceneLayer : public Layer {

public:
    virtual void handle_attach() override;

    virtual void handle_update() override;

    virtual void handle_draw() override;

    virtual void handle_deattach() override;

    virtual void handle_event(Event &e) override;

private:

    //bool handle_viewport_changed(ViewportChangedEvent &viewport);

    inline float lerp(float v1, float v2, float t){
        return v2 * t + (1 - t) * v1;
    }

     inline glm::vec4 lerp(glm::vec4 c1, glm::vec4 c2, float t){
         glm::vec4 result = {};
         result[0] = lerp(c1[0], c2[0], t);
         result[1] = lerp(c1[1], c2[1], t);
         result[2] = lerp(c1[2], c2[2], t);
         result[3] = lerp(c1[3], c2[3], t);
         return result;
    }

    inline glm::vec3 lerp(glm::vec3 v1, glm::vec3 v2, float t){
        return glm::vec3(lerp(v1.x, v2.x, t), lerp(v1.z, v2.z, t), lerp(v1.z, v2.z, t));
    }

private:

    struct {
        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;
    } sceneViewport;

    EventDispatcher m_dispatcher;
    std::unique_ptr<Scene> m_scene;
    entityx::EventManager m_events;
    std::shared_ptr<entityx::EntityManager> m_entities;
    std::shared_ptr<RenderingContext> m_renderingContext;
};

EG_ENGINE_END

#endif //EAGLE_SCENELAYER_H
