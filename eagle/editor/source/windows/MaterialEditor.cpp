//
// Created by Novak on 01/09/2019.
//

#include "eagle/editor/windows/MaterialEditor.h"

EG_EDITOR_BEGIN


MaterialEditor::MaterialEditor(Object *object)  {
    material = ResourceHandle<Material>(*object);
}


void MaterialEditor::handle_update() {

    ImGui::Text("Shader  ");
    ImGui::SameLine();
    ImGui::Text("%s", material->shader_module()->name().c_str());

    for (auto& kv : material->bindings()){

        const DescriptorBindingDescription& item = kv.second;

        if (item.descriptorType == Eagle::DescriptorType::IMAGE_2D) {

            ImGui::Text("%s", item.name.c_str());
            ImGui::SameLine();
            ImGui::Button(material->get_texture(item.name)->name().c_str());
            if (ImGui::BeginDragDropTarget()){
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Resource<Texture>::static_type_name().c_str())){
                    material->set_texture(item.name, ResourceHandle<Texture>(*static_cast<ResourceID*>(payload->Data)));
                    material->update();
                }
                ImGui::EndDragDropTarget();
            }
        }
        else{
            if (ImGui::TreeNode(item.name.c_str(), "%s", item.name.c_str())){
                ImGui::Text("Buffer size: %zu", item.size);
                for( auto& itemKv : item.members){

                    auto member = itemKv.second;

                    ImGui::Text("%s", member.name.c_str());
                    ImGui::SameLine();
                    switch(member.type){
                        case DataType::UNDEFINED:ImGui::Text("UNDEFINED");break;
                        case DataType::BOOL:ImGui::Text("BOOL");break;
                        case DataType::INT:ImGui::Text("INT");break;
                        case DataType::FLOAT:{
                            float value = material->get_uniform<float>(item.name, member.name);
                            ImGui::SameLine();
                            if (ImGui::InputFloat(member.name.c_str(), &value)) {
                                material->set_uniform<float>(item.name, member.name, value);
                            }
                            break;
                        }
                        case DataType::VECTOR2F:ImGui::Text("VECTOR2F");break;
                        case DataType::VECTOR3F:{
                            glm::vec3 value = material->get_uniform<glm::vec3>(item.name, member.name);
                            ImGui::SameLine();
                            if (ImGui::ColorEdit3(member.name.c_str(), &value[0])) {
                                material->set_uniform<glm::vec3>(item.name, member.name, value);
                            }
                            break;
                        }
                        case DataType::VECTOR4F:ImGui::Text("VECTOR4F");break;
                        case DataType::VECTOR2I:ImGui::Text("VECTOR2I");break;
                        case DataType::VECTOR3I:ImGui::Text("VECTOR3I");break;
                        case DataType::VECTOR4I:ImGui::Text("VECTOR4I");break;
                        case DataType::MATRIX2X2:ImGui::Text("MATRIX2X2");break;
                        case DataType::MATRIX3X3:ImGui::Text("MATRIX3X3");break;
                        case DataType::MATRIX4X4:ImGui::Text("MATRIX4X4");break;
                        case DataType::EXTERNAL_IMAGE:ImGui::Text("EXTERNAL_IMAGE");break;
                        case DataType::EXTERNAL_SAMPLER:ImGui::Text("EXTERNAL_SAMPLER");break;
                        case DataType::EXTERNAL_SAMPLED_IMAGE:ImGui::Text("EXTERNAL_SAMPLED_IMAGE");break;
                        case DataType::EXTERNAL_BLOCK:ImGui::Text("EXTERNAL_BLOCK");break;
                        case DataType::EXTERNAL_MASK:ImGui::Text("EXTERNAL_MASK");break;
                        case DataType::STRUCT:ImGui::Text("STRUCT");break;
                        case DataType::ARRAY:ImGui::Text("ARRAY");break;
                    }
                    ImGui::Text("Size: %i  -  Offset: %i", member.size, member.offset);
                    ImGui::Spacing();
                }
                ImGui::TreePop();
            }
        }
    }
}

EG_EDITOR_END
