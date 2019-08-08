//
// Created by Novak on 06/08/2019.
//

#include "eagle/engine/editor/InspectorWindow.h"
#include "eagle/engine/editor/Selection.h"
#include <imgui.h>
#include <eagle/engine/graphics/Transform.h>

EG_ENGINE_BEGIN

InspectorWindow::InspectorWindow() : EditorWindow("Inspector"){

}

InspectorWindow::~InspectorWindow() {

}

void InspectorWindow::handle_update() {
    if (Selection::current.valid()) {
        std::string name = "Entity " + std::to_string(Selection::current.id().id());
        ImGui::Text("%s", name.c_str());
        if (ImGui::TreeNode("Transform:")) {

            auto transform = Selection::current.component<Transform>();

            ImGui::DragFloat3("Position", (float *) &transform->position, 0.1f);

            glm::vec3 rotation = transform->get_euler();
            if (ImGui::DragFloat3("Rotation", (float *) &rotation, 0.2f)) {
                transform->set_euler(rotation);
            }

            ImGui::DragFloat3("Scale", (float *) &transform->scale, 0.1f);

            ImGui::TreePop();
        }
    }
}

EG_ENGINE_END


