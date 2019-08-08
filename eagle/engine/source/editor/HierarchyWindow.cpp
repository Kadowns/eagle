//
// Created by Novak on 06/08/2019.
//

#include <imgui.h>
#include "eagle/engine/editor/Selection.h"
#include "eagle/engine/editor/HierarchyWindow.h"

EG_ENGINE_BEGIN

HierarchyWindow::HierarchyWindow(const std::shared_ptr<entityx::EntityManager> &entities) :
    EditorWindow("Hierarchy"),
    m_entities(entities) {

}

HierarchyWindow::~HierarchyWindow() {

}

void HierarchyWindow::handle_update() {
    if (ImGui::TreeNode("Entities on scene")) {
        for (auto e : m_entities->entities_for_debugging()) {

            ImGui::PushID(e.id().id());

            char buf[32];
            sprintf(buf, "Object %llu", e.id().id());
            if (ImGui::Selectable(buf, Selection::current == e))
                Selection::current = e;

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

EG_ENGINE_END