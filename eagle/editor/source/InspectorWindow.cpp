//
// Created by Novak on 06/08/2019.
//

#include "eagle/editor/InspectorWindow.h"
#include "eagle/editor/Selection.h"
#include <imgui/imgui.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>

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

            ImGui::InputFloat3("Position", (float *) &transform->position, 3);


            glm::quat rot = transform->rotation;
            if (ImGui::SliderFloat4("Rotation", (float *) &rot, -1.0f, 1.0f)){
                transform->rotation = glm::normalize(rot);
            }

            ImGui::InputFloat3("Scale", (float *) &transform->scale, 3);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Renderable:")) {

            auto renderable = Selection::current.component<Renderable>();

            ImGui::Text("Mesh");
            ImGui::SameLine();
            if (!renderable->mesh){
                name = "None";
            }else{
                name = renderable->mesh->name();
            }
            ImGui::Button(name.c_str());
            if (ImGui::BeginDragDropTarget()){
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Resource<Mesh>::static_type_name().c_str())){
                    renderable->mesh = ResourceHandle<Mesh>(*static_cast<ResourceID*>(payload->Data));
                }
                ImGui::EndDragDropTarget();
            }


            ImGui::Text("Material");
            ImGui::SameLine();
            if (!renderable->material){
                name = "None";
            }else{
                name = renderable->material->name();
            }
            ImGui::Button(name.c_str());
            if (ImGui::BeginDragDropTarget()){
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Resource<Material>::static_type_name().c_str())){
                    renderable->material = ResourceHandle<Material>(*static_cast<ResourceID*>(payload->Data));
                }
                ImGui::EndDragDropTarget();
            }


            ImGui::TreePop();
        }
    }
}

EG_ENGINE_END


