//
// Created by Novak on 22/03/2020.
//

#include <eagle/editor/window/EditorWindow.h>

EG_EDITOR_BEGIN

EditorWindow::EditorWindow(const std::string &name) : m_name(name) {

}

void EditorWindow::handle_update() {
    if (ImGui::Begin(m_name.c_str())){
        m_isHovered = ImGui::IsWindowHovered();
        m_isFocused = ImGui::IsWindowFocused();
        handle_window_update();
    }
    ImGui::End();
}

EG_EDITOR_END


