//
// Created by Novak on 13/08/2019.
//

#include "eagle/editor/ResourcesWindow.h"
#include "eagle/engine/resources/ResourcesPool.h"
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

EG_ENGINE_BEGIN

ResourcesWindow::ResourcesWindow() : EditorWindow("Resources") {
}

ResourcesWindow::~ResourcesWindow() {

}

void ResourcesWindow::handle_update() {
//    auto files = ResourceManager::instance().get_paths();
//    auto &&it = files.begin();

//    while (it != get_paths.end()) {
//
//        if (boost::filesystem::is_directory(*it)) {
//    show_directory(files, it, true);
//        } else {
//            ImGui::Text("%s", it->filename().string().c_str());
//            ++it;
//        }
//    }

    for (auto& family : ResourcesPool::instance().resources()){
        for (auto& resource : family){
            //ImGui::PushID(resource.first);
            ImGui::Selectable(resource.second->name().c_str());
            if (ImGui::BeginDragDropSource()){

                ImGui::SetDragDropPayload(resource.second->type_name().c_str(), &resource.first, sizeof(resource.first));
                ImGui::TextUnformatted(resource.second->name().c_str());
                ImGui::EndDragDropSource();
            }
            //ImGui::PopID();
        }
    }
}


void Eagle::Engine::ResourcesWindow::show_directory(const std::vector<boost::filesystem::path> &files,
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

EG_ENGINE_END


