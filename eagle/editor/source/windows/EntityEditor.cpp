//
// Created by Novak on 02/09/2019.
//

#include <eagle/editor/windows/EntityEditor.h>

EG_EDITOR_BEGIN

EntityEditor::EntityEditor(Object *object) {
    entity = dynamic_cast<Entity*>(object);
}

void EntityEditor::handle_update() {

    ImGui::Text("%s", entity->name().c_str());
    if (ImGui::TreeNode("Transform:")) {

        auto transform = entity->component<Transform>();

        ImGui::InputFloat3("Position", (float *) &transform->position, 3);


        glm::quat rot = transform->rotation;
        if (ImGui::SliderFloat4("Rotation", (float *) &rot, -1.0f, 1.0f)){
            transform->rotation = glm::normalize(rot);
        }

        ImGui::InputFloat3("Scale", (float *) &transform->scale, 3);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Renderable:")) {

        auto renderable = entity->component<Renderable>();

        ImGui::Text("Mesh");
        ImGui::SameLine();
        std::string name;
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

    if (entity->has_component<DirectionalLight>()) {

        if (ImGui::TreeNode("Directional Light:")) {

            bool changed = false;

            auto light = entity->component<DirectionalLight>();
            if (ImGui::SliderFloat3("Direction", (float *) &light->direction, -1, 1)){
                light->direction = glm::normalize(light->direction);
                changed |= true;
            }
            changed |= ImGui::ColorEdit3("Color", (float *) &light->color);
            changed |= ImGui::SliderFloat("Scale", (float *) &light->intensity, 0, 1);

            if (changed){
                LightSettings::instance().set_directional_light(*light);
                LightSettings::instance().update();
            }

            ImGui::TreePop();
        }
    }
}

EG_EDITOR_END