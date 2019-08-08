//
// Created by Novak on 14/07/2019.
//

#ifndef EAGLE_EDITORLAYER_H
#define EAGLE_EDITORLAYER_H

#include <eagle/engine/graphics/Scene.h>
#include "editor/EditorWindow.h"
#include "EngineCore.h"


EG_ENGINE_BEGIN

class EditorLayer : public Eagle::Layer {

private:

    struct PushConstBlock {
        glm::vec2 scale;
        glm::vec2 translate;
    } pushConstBlock;

public:

    virtual void handle_attach() override;
    virtual void handle_update() override;
    virtual void handle_draw() override;
    virtual void handle_deattach() override;
    virtual void handle_event(Event& e) override;

private:

    void submit_buffers();
    void update_mouse_cursor();
    bool handle_window_resized(WindowResizedEvent &e);

    bool handle_mouse_moved(MouseMoveEvent& e);
    bool handle_mouse_button_pressed(MousePressedEvent &e);
    bool handle_mouse_button_released(MouseReleasedEvent &e);
    bool handle_mouse_scrolled(MouseScrolledEvent& e);
    bool handle_key(KeyEvent &e);
    bool handle_key_typed(KeyTypedEvent& e);


private:
    EventDispatcher m_dispatcher;
    std::weak_ptr<entityx::EntityManager> m_entities;
    std::weak_ptr<Texture2D> m_font;
    std::weak_ptr<Shader> m_shader;
    std::weak_ptr<DescriptorSetLayout> m_descriptorLayout;
    std::weak_ptr<DescriptorSet> m_descriptor;

    std::weak_ptr<IndexBuffer> m_indexBuffer;
    std::weak_ptr<VertexBuffer> m_vertexBuffer;

    std::vector<std::shared_ptr<EditorWindow>> m_editors;

    std::shared_ptr<RenderingContext> m_renderingContext;

};

EG_ENGINE_END

#endif //EAGLE_EDITORLAYER_H
