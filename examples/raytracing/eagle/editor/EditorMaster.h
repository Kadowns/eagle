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

    bool receive(const Engine::OnRenderContextInit& ev);
    bool receive(const Engine::OnRenderCommandBufferMainRenderPass& ev);

private:
    void init_imgui();
    void init_render_state(const Reference <Shader> &shader, CommandBuffer *commandBuffer,
                           ImDrawData *drawData);
    void update_mouse_cursor();
    void update_buffers();

private:

    Handle<Texture> m_font;
    Handle<Shader> m_blendEnabledShader, m_blendDisabledShader;
    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
    Handle<IndexBuffer> m_indexBuffer;
    Handle<VertexBuffer> m_vertexBuffer;
    EventListener<EditorMaster> m_listener;

    static std::vector<Reference<EditorWindow>> m_windows;
};

EG_EDITOR_END

#endif //EAGLE_EDITORMASTER_H
