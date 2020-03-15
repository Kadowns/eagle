//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_SCENE_H
#define EAGLE_SCENE_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/LightSettings.h>
#include <eagle/engine/Camera.h>
#include <eagle/engine/EntityManager.h>
#include "ResourcesPool.h"

EG_ENGINE_BEGIN

class Scene : public Resource<Scene> {
public:

    explicit Scene(const std::string& name);
    ~Scene();

    void update();
    void render(Scope <CommandBuffer> &commandBuffer, const Reference <Camera> &camera);

    Reference<EntityManager>& entities() { return m_entities; }
    const Reference<EntityManager>& entities() const { return m_entities; }


private:
    Reference<EntityManager> m_entities;
    LightSettings m_light;

};

EG_ENGINE_END

#endif //EAGLE_SCENE_H
