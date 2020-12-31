//
// Created by Novak on 12/04/2020.
//

#ifndef EAGLE_SCENELAYER_H
#define EAGLE_SCENELAYER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

struct OnSceneRecreate {

};

EG_ENGINE_BEGIN

class SceneLayer : public Layer {
public:
    virtual void handle_attach(EventBus<EventStream>* eventBus) override;

    virtual void handle_detach() override;

    virtual void handle_update() override;

private:
    void generate_scene();
    void generate_playground();
    glm::vec2 random_inside_unit_circle();
private:
    EventListener<SceneLayer, EventBus<EventStream>> m_listener;
};

EG_ENGINE_END

#endif //EAGLE_SCENELAYER_H
