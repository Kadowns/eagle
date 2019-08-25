//
// Created by Novak on 06/08/2019.
//

#include <imgui/imgui.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>
#include "eagle/editor/Selection.h"
#include "eagle/editor/HierarchyWindow.h"

EG_ENGINE_BEGIN

HierarchyWindow::HierarchyWindow(const Reference<entityx::EntityManager> &entities) :
    EditorWindow("Hierarchy"),
    m_entities(entities) {

}

HierarchyWindow::~HierarchyWindow() {

}

void HierarchyWindow::handle_update() {


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
            if (ImGui::Selectable(buf, Selection::current == e))
                Selection::current = e;

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

EG_ENGINE_END