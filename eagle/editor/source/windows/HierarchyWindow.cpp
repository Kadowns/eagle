//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/windows/HierarchyWindow.h>
#include <eagle/editor/Selection.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>

EG_EDITOR_BEGIN

HierarchyWindow::HierarchyWindow() :
    EditorWindow("Hierarchy"){

}

HierarchyWindow::~HierarchyWindow() {

}

void HierarchyWindow::handle_update() {

    if (!SceneManager::instance().is_active()){
        ImGui::Text("No active scene");
        return;
    }

    if (!m_entities){
        m_entities = SceneManager::current_scene().entities();
    }


    if (ImGui::Button("Create Entity")){
        auto e = m_entities->create();
        e->assign<Transform>();
        e->assign<Renderable>();
    }


    if (ImGui::TreeNode("Entities on scene")) {
        for (auto e : m_entities->entities()) {

            ImGui::PushID(e->uid());

            char buf[32];
            sprintf(buf, "%s", e->name().c_str());

            if (ImGui::Selectable(buf, Selection::is_selected(e.get()))) {
                Selection::set_selected(e.get());
            }

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

EG_EDITOR_END