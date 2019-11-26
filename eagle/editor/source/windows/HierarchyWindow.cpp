//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/Selection.h>
#include <eagle/editor/windows/HierarchyWindow.h>
#include <eagle/editor/EditorSceneManager.h>

EG_EDITOR_BEGIN

HierarchyWindow::HierarchyWindow() :
    EditorWindow("Hierarchy"){

}

HierarchyWindow::~HierarchyWindow() {

}

void HierarchyWindow::handle_update() {

    if (!EditorSceneManager::instance().is_active()){
        ImGui::Text("No active scene");
        return;
    }

    if (!m_entities){
        m_entities = SceneManager::current_scene().entities();
    }


    if (ImGui::Button("Create Entity")){
        auto e = m_entities->create();
        e.assign<Transform>();
        e.assign<Renderable>();
    }


    if (ImGui::TreeNode("Entities on scene")) {
        for (auto e : m_entities->entities_for_debugging()) {

            ImGui::PushID(e.id().id());

            char buf[32];
            sprintf(buf, "Object %llu", e.id().id());
            if (ImGui::Selectable(buf, Selection::current_entity() == e))
                Selection::set_selected(e);

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

EG_EDITOR_END