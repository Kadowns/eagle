//
// Created by Novak on 22/03/2020.
//

#include <eagle/editor/EditorLayer.h>
#include <eagle/editor/window/EditorDockingWindow.h>
#include <eagle/editor/window/SceneWindow.h>
#include <eagle/editor/window/DebugSettingsWindow.h>

EG_EDITOR_BEGIN

EditorLayer::EditorLayer() {
    //EditorMaster::add_window(std::make_shared<EditorDockingWindow>());

}

EditorLayer::~EditorLayer() {

}

void EditorLayer::handle_attach() {

    EditorMaster::add_window(std::make_shared<DebugSettingsWindow>());

    m_listener.attach(&Application::instance().event_bus());

    m_listener.subscribe<OnMouseMove>([this](const OnMouseMove& ev){
        return m_editorMaster.handle_mouse_moved(ev);
    });

    m_listener.subscribe<OnMouseButton>([this](const OnMouseButton& ev){
        return m_editorMaster.handle_mouse_button(ev);
    });

    m_listener.subscribe<OnMouseScrolled>([this](const OnMouseScrolled& ev){
        return m_editorMaster.handle_mouse_scrolled(ev);
    });

    m_listener.subscribe<OnKeyTyped>([this](const OnKeyTyped& ev){
        return m_editorMaster.handle_key_typed(ev);
    });

    m_listener.subscribe<OnKey>([this](const OnKey& ev){
        return m_editorMaster.handle_key(ev);
    });

    m_listener.subscribe<OnWindowResized>([this](const OnWindowResized& ev){
        return m_editorMaster.handle_window_resized(ev);
    });

    m_editorMaster.init();
}

void EditorLayer::handle_detach() {
    m_editorMaster.deinit();
    m_listener.detach();
}

void EditorLayer::handle_update() {
    m_editorMaster.update();
}

EG_EDITOR_END
