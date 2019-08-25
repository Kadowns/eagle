//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_SCENEWINDOW_H
#define EAGLE_SCENEWINDOW_H

#include <eagle/engine/Camera.h>
#include <eagle/engine/Scene.h>
#include <eagle/engine/components/Transform.h>
#include "EditorWindow.h"


EG_ENGINE_BEGIN

class SceneWindow : public EditorWindow {
public:

    SceneWindow();
    virtual ~SceneWindow();

    virtual void handle_update() override;
    virtual void draw() override;

    inline Reference<Scene>& scene() { return m_scene; }

private:

    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
    Reference<Camera> m_camera;
    Reference<Scene> m_scene;

    Transform m_cameraTransform;
};

EG_ENGINE_END

#endif //EAGLE_SCENEWINDOW_H
