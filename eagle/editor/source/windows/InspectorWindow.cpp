//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/windows/InspectorWindow.h>
#include <eagle/editor/Selection.h>

EG_EDITOR_BEGIN

InspectorWindow::InspectorWindow() : EditorWindow("Inspector"){

}

InspectorWindow::~InspectorWindow() {

}

void InspectorWindow::handle_update() {
    if (Selection::current_entity()) {
        draw_entity_inspector(Selection::current_entity());
    }
    else if (Selection::current_resource()){
        draw_resource_inspector(Selection::current_resource());
    }
}

void InspectorWindow::draw_entity_inspector(entityx::Entity e) {
    std::string name = "Entity " + std::to_string(e.id().id());
    ImGui::Text("%s", name.c_str());
    if (ImGui::TreeNode("Transform:")) {

        auto transform = e.component<Transform>();

        ImGui::InputFloat3("Position", (float *) &transform->position, 3);


        glm::quat rot = transform->rotation;
        if (ImGui::SliderFloat4("Rotation", (float *) &rot, -1.0f, 1.0f)){
            transform->rotation = glm::normalize(rot);
        }

        ImGui::InputFloat3("Scale", (float *) &transform->scale, 3);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Renderable:")) {

        auto renderable = e.component<Renderable>();

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

void InspectorWindow::draw_resource_inspector(BaseResource *resource) {

    ImGui::LabelText(resource->type_name().c_str(), "%s", resource->name().c_str());

    if (Resource<Material>* material = dynamic_cast<Resource<Material>*>(resource)){
        ResourceHandle<Material> handle(*material);
        for (auto& item : handle->bindings()){
            if (item.second.descriptorType == EgDescriptorType::IMAGE_2D) {

                ImGui::Text("%s", item.first.c_str());
                ImGui::SameLine();
                ImGui::Button(handle->get_texture(item.first)->name().c_str());
                if (ImGui::BeginDragDropTarget()){
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Resource<Texture>::static_type_name().c_str())){
                        handle->set_texture(item.first, ResourceHandle<Texture>(*static_cast<ResourceID*>(payload->Data)));
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
    }

}

EG_EDITOR_END


