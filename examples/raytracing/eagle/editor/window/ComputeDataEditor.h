//
// Created by Novak on 04/04/2020.
//

#ifndef EAGLE_COMPUTEDATAEDITOR_H
#define EAGLE_COMPUTEDATAEDITOR_H

#include <eagle/editor/EditorGlobalDefinitions.h>
#include <eagle/editor/window/EditorWindow.h>

EG_EDITOR_BEGIN

class ComputeDataEditor : public EditorWindow {
public:
    ComputeDataEditor(Engine::SceneData &data, const std::function<void()> &recreateSpheresCallback);

    virtual void handle_window_update() override;

private:
    Engine::SceneData& m_data;
    std::function<void()> recreate_spheres;
};


EG_EDITOR_END

#endif //EAGLE_COMPUTEDATAEDITOR_H
