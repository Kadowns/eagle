//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_SCENEWINDOW_H
#define EAGLE_SCENEWINDOW_H

#include "EditorWindow.h"

EG_EDITOR_BEGIN

class SceneWindow : public EditorWindow {
public:

    SceneWindow();
    virtual ~SceneWindow();

    virtual void handle_update() override;
    virtual void draw(Scope <CommandBuffer> &commandBuffer) override;

private:

    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
    Reference<Camera> m_camera;

    Transform m_cameraTransform;
};

EG_EDITOR_END

#endif //EAGLE_SCENEWINDOW_H
