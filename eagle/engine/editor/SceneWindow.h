//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_SCENEWINDOW_H
#define EAGLE_SCENEWINDOW_H

#include <eagle/engine/graphics/Camera.h>
#include <eagle/engine/graphics/Scene.h>
#include "eagle/engine/editor/EditorWindow.h"


EG_ENGINE_BEGIN

class SceneWindow : public EditorWindow {
public:

    SceneWindow();
    virtual ~SceneWindow();

    virtual void handle_update() override;
    virtual void draw() override;

    inline std::shared_ptr<Scene>& scene() { return m_scene; }

private:


    std::weak_ptr<DescriptorSetLayout> m_descriptorLayout;
    std::weak_ptr<DescriptorSet> m_descriptor;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<Scene> m_scene;

};

EG_ENGINE_END

#endif //EAGLE_SCENEWINDOW_H
