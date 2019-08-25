//
// Created by Novak on 14/07/2019.
//

#ifndef EAGLE_EDITORLAYER_H
#define EAGLE_EDITORLAYER_H

#include <eagle/engine/resources/ResourcesPool.h>
#include "../engine/Scene.h"
#include "EditorWindow.h"
#include "../engine/EngineCore.h"


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
    bool handle_mouse_button(MouseButtonEvent &e);
    bool handle_mouse_scrolled(MouseScrolledEvent& e);
    bool handle_key(KeyEvent &e);
    bool handle_key_typed(KeyTypedEvent& e);


private:
    EventDispatcher m_dispatcher;
    Handle<Texture2D> m_font;
    Handle<Shader> m_shader;
    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;

    Handle<IndexBuffer> m_indexBuffer;
    Handle<VertexBuffer> m_vertexBuffer;

    //ResourcesPool m_resources;

    std::vector<Reference<EditorWindow>> m_editors;

    Reference<RenderingContext> m_renderingContext;

};

EG_ENGINE_END

#endif //EAGLE_EDITORLAYER_H
