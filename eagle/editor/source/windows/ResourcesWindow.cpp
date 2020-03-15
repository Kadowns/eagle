//
// Created by Novak on 13/08/2019.
//

#include <eagle/editor/windows/ResourcesWindow.h>
#include <eagle/editor/Selection.h>

EG_EDITOR_BEGIN

ResourcesWindow::ResourcesWindow() : EditorWindow("Resources") {
}

ResourcesWindow::~ResourcesWindow() {

}

void ResourcesWindow::handle_update() {

    if (ImGui::BeginPopupContextWindow("Create")){
        ImGui::Text("Create:");
        if (ImGui::MenuItem("Material")){

        }
        if (ImGui::MenuItem("Shader")){

        }

        ImGui::EndPopup();
    }


    for (auto& family : ResourcesPool::instance().resources()){

        if (ImGui::TreeNode(family.begin()->second->type_name().c_str())) {


            for (auto &resource : family) {
                //ImGui::PushID(resource.first);
                if (ImGui::Selectable(resource.second->name().c_str(), Selection::is_selected(resource.second))) {
                    Selection::set_selected(resource.second);


                }
                if (ImGui::BeginDragDropSource()) {

                    ImGui::SetDragDropPayload(resource.second->type_name().c_str(), &resource.first,
                                              sizeof(resource.first));
                    ImGui::TextUnformatted(resource.second->name().c_str());
                    ImGui::EndDragDropSource();
                }
                //ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }
}


void ResourcesWindow::show_directory(const std::vector<boost::filesystem::path> &files,
                                                    std::vector<boost::filesystem::path>::iterator &it, bool foldout) {

    if (foldout){
        foldout = ImGui::TreeNode((*it).filename().string().c_str());
    }
    boost::filesystem::path currentDirectory = *it;
    ++it;
    while(it != files.end()){

        if ((*it).string().find(currentDirectory.string()) == std::string::npos){
            break;
        }

        if (boost::filesystem::is_directory(*it)){
            show_directory(files, it, foldout);
        }
        else {
            if (foldout){
                ImGui::Text("%s", it->filename().string().c_str());
            }
            ++it;
        }
    }

    if (foldout){
        ImGui::TreePop();
    }
}

EG_EDITOR_END


