//
// Created by Novak on 21/03/2020.
//

#ifndef EAGLE_EDITORMASTER_H
#define EAGLE_EDITORMASTER_H

#include <eagle/editor/EditorGlobalDefinitions.h>
#include <eagle/editor/window/EditorWindow.h>

EG_EDITOR_BEGIN

class EditorMaster {
public:
    struct PushConstBlock {
        glm::vec2 scale;
        glm::vec2 translate;
    } pushConstBlock;
public:
    EditorMaster();
    ~EditorMaster();

    void init();
    void deinit();
    void update();

    static void add_window(const Reference<EditorWindow>& window);
    static void remove_window(const Reference<EditorWindow>& window);

    bool handle_window_resized(const OnWindowResized &e);
    bool handle_mouse_moved(const OnMouseMove &e);
    bool handle_mouse_button(const OnMouseButton &e);
    bool handle_mouse_scrolled(const OnMouseScrolled &e);
    bool handle_key(const OnKey &e);
    bool handle_key_typed(const OnKeyTyped &e);

private:
    void init_imgui();
    void handle_context_init(RenderingContext &context);
    void handle_command_buffer_main_render_pass(Reference<CommandBuffer> &commandBuffer);
    void init_render_state(const Reference <Shader> &shader, Reference <CommandBuffer> &commandBuffer,
                           ImDrawData *drawData);
    void update_mouse_cursor();
    void update_buffers();

private:
    Engine::RenderMaster::CommandBufferEvent::Listener handle_command_buffer_main_render_pass_callback;
    Engine::RenderMaster::Event::Listener handle_context_init_callback;

    Handle<Texture> m_font;
    Handle<Shader> m_blendEnabledShader, m_blendDisabledShader;
    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
    Handle<IndexBuffer> m_indexBuffer;
    Handle<VertexBuffer> m_vertexBuffer;

    static std::vector<Reference<EditorWindow>> m_windows;
};

EG_EDITOR_END

#endif //EAGLE_EDITORMASTER_H
