//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/windows/EditorWindow.h>

EG_EDITOR_BEGIN

EditorWindow::EditorWindow(const std::string &name) :
    m_name(name) {

}

EditorWindow::~EditorWindow() {

}

void EditorWindow::update() {
    if (ImGui::Begin(m_name.c_str())){
        handle_update();
    }
    ImGui::End();
}


EG_EDITOR_END