//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/windows/InspectorWindow.h>
#include <eagle/editor/windows/InspectorConfig.h>
#include <eagle/editor/Selection.h>

EG_EDITOR_BEGIN

InspectorWindow::InspectorWindow() : EditorWindow("Inspector"){
    m_onObjectSelectedCallback = [&](Object* object){
        m_currentEditor.reset(EditorFactory::create_for_object(object));
    };
    Selection::on_object_selected += &m_onObjectSelectedCallback;
}

InspectorWindow::~InspectorWindow() {
    Selection::on_object_selected -= &m_onObjectSelectedCallback;
}

void InspectorWindow::handle_update() {
    if (!m_currentEditor){
        ImGui::TextUnformatted("Nothing to show");
        return;
    }

    m_currentEditor->handle_update();
}

EG_EDITOR_END
