//
// Created by Novak on 16/07/2019.
//
#include "eagle/engine/EditorLayer.h"

#include "eagle/engine/editor/InspectorWindow.h"
#include "eagle/engine/editor/HierarchyWindow.h"
#include "eagle/engine/editor/SceneWindow.h"


#include <imgui.h>
#include <eagle/engine/graphics/Transform.h>

EG_ENGINE_BEGIN

void EditorLayer::handle_attach() {

    m_dispatcher.add_listener<MousePressedEvent>([&](Event &e) {
        return handle_mouse_button_pressed(*(MousePressedEvent*) &e);
    });
    m_dispatcher.add_listener<MouseMoveEvent>([&](Event &e) {
        return handle_mouse_moved(*(MouseMoveEvent*) &e);
    });
    m_dispatcher.add_listener<MousePressedEvent>([&](Event &e) {
        return handle_mouse_button_pressed(*(MousePressedEvent*) &e);
    });
    m_dispatcher.add_listener<MouseReleasedEvent>([&](Event &e) {
        return handle_mouse_button_released(*(MouseReleasedEvent*) &e);
    });
    m_dispatcher.add_listener<MouseScrolledEvent>([&](Event &e) {
        return handle_mouse_scrolled(*(MouseScrolledEvent*) &e);
    });
    m_dispatcher.add_listener<KeyTypedEvent>( [&](Event &e) {
        return handle_key_typed(*(KeyTypedEvent*) &e);
    });
    m_dispatcher.add_listener<KeyEvent>([&](Event &e) {
        return handle_key(*(KeyEvent*) &e);
    });
    m_dispatcher.add_listener<WindowResizedEvent>([&](Event &e) {
        return handle_window_resized(*(WindowResizedEvent*) &e);
    });



    ImGui::CreateContext();

    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.26f, 0.25f, 0.7f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.3f, 0.26f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.16f, 0.15f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.32f, 0.3f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.115f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1f, 0.115f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.1f, 0.115f, 0.1f, 1.0f);

    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)

    io.KeyMap[ImGuiKey_Tab] = EG_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = EG_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = EG_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = EG_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = EG_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = EG_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = EG_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = EG_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = EG_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = EG_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = EG_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = EG_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = EG_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = EG_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = EG_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = EG_KEY_A;
    io.KeyMap[ImGuiKey_C] = EG_KEY_C;
    io.KeyMap[ImGuiKey_V] = EG_KEY_V;
    io.KeyMap[ImGuiKey_X] = EG_KEY_X;
    io.KeyMap[ImGuiKey_Y] = EG_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = EG_KEY_Z;


    io.DisplaySize = ImVec2(Application::instance().get_window()->get_width(), Application::instance().get_window()->get_height());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


    m_renderingContext = Application::instance().get_window()->get_rendering_context();
    
    Pixel *pixels;
    Texture2DCreateInfo fontCreateInfo = {};
    io.Fonts->GetTexDataAsRGBA32(&pixels, &fontCreateInfo.width, &fontCreateInfo.height);
    fontCreateInfo.format = EG_FORMAT_R8G8B8A8_UNORM;
    fontCreateInfo.layerCount = 1;
    fontCreateInfo.mipLevels = 1;
    fontCreateInfo.channels = 4;
    fontCreateInfo.pixels = std::vector<Pixel>(pixels,
                                                      pixels + fontCreateInfo.width * fontCreateInfo.height * 4);

    m_font = m_renderingContext->create_texture_2d(fontCreateInfo);


    VertexLayout vertexLayout(5, {
            EG_FORMAT_R32G32_SFLOAT,
            EG_FORMAT_R32G32_SFLOAT,
            EG_FORMAT_R8G8B8A8_UNORM
    });


    DescriptorBinding binding = {};
    binding.shaderStage = EG_SHADER_STAGE::FRAGMENT;
    binding.descriptorType = EG_DESCRIPTOR_TYPE::IMAGE_2D;
    binding.binding = 0;

    m_descriptorLayout = m_renderingContext->create_descriptor_set_layout({binding});


    ShaderPipelineInfo pipelineInfo(vertexLayout);
    pipelineInfo.blendEnable = true;
    pipelineInfo.dynamicStates = true;
    pipelineInfo.depthTesting = false;

    m_shader = m_renderingContext->create_shader(ProjectRoot + "shaders/text.vert",
                                                 ProjectRoot + "shaders/text.frag", {m_descriptorLayout.lock()}, pipelineInfo);

    m_descriptor = m_renderingContext->create_descriptor_set(m_descriptorLayout.lock(), {m_font.lock()->get_image().lock()});

    io.Fonts->TexID = (ImTextureID)&m_descriptor;

    m_vertexBuffer = m_renderingContext->create_vertex_buffer(nullptr, 0, vertexLayout,
                                                                   EG_BUFFER_USAGE::DYNAMIC);
    m_indexBuffer = m_renderingContext->create_index_buffer(nullptr, 0, INDEX_BUFFER_TYPE::UINT_16,
                                                                 EG_BUFFER_USAGE::DYNAMIC);

    m_editors.push_back(std::make_shared<InspectorWindow>());
    m_editors.push_back(std::make_shared<SceneWindow>());
    m_editors.push_back(std::make_shared<HierarchyWindow>(std::static_pointer_cast<SceneWindow>(m_editors.back())->scene()->entities()));

}

void EditorLayer::handle_update() {

    //m_scene->update();


    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    //process even if the dockspace is collapsed, so child windows will not lose their parents
    ImGui::Begin("Dock", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();

    for (auto& editor : m_editors){
        editor->update();
    }

    ImGui::Render();

    update_mouse_cursor();
    submit_buffers();
}

void EditorLayer::handle_draw() {


    for (auto& editor : m_editors){
        editor->draw();
    }

    ImGuiIO &io = ImGui::GetIO();
    ImDrawData *imDrawData = ImGui::GetDrawData();

    // Render commands
    if (imDrawData->CmdListsCount > 0) {
        m_renderingContext->bind_shader(m_shader.lock());

        m_renderingContext->set_viewport(io.DisplaySize.x, io.DisplaySize.y, 0, 0, 0, 1);

        pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
        pushConstBlock.translate = glm::vec2(-1.0f);

        m_renderingContext->push_constants(m_shader.lock(), EG_SHADER_STAGE::VERTEX, 0,
                                                sizeof(pushConstBlock), &pushConstBlock);

        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        m_renderingContext->bind_vertex_buffer(m_vertexBuffer.lock());
        m_renderingContext->bind_index_buffer(m_indexBuffer.lock());

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {

            const ImDrawList *cmdList = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmdList->CmdBuffer.Size; j++) {

                const ImDrawCmd *cmd = &cmdList->CmdBuffer[j];
                uint32_t x = std::max((int32_t) (cmd->ClipRect.x), 0);
                uint32_t y = std::max((int32_t) (cmd->ClipRect.y), 0);
                uint32_t w = (uint32_t) (cmd->ClipRect.z - cmd->ClipRect.x);
                uint32_t h = (uint32_t) (cmd->ClipRect.w - cmd->ClipRect.y);
                m_renderingContext->set_scissor(w, h, x, y);

                m_renderingContext->bind_descriptor_sets(m_shader.lock(), static_cast<std::weak_ptr<DescriptorSet>*>(cmd->TextureId)->lock(), 0);

                m_renderingContext->draw_indexed(cmd->ElemCount, indexOffset, vertexOffset);

                indexOffset += cmd->ElemCount;
            }
            vertexOffset += cmdList->VtxBuffer.Size;
        }
    }
}

void EditorLayer::handle_deattach() {
    ImGui::DestroyContext();
}

void EditorLayer::submit_buffers() {
    ImDrawData *imDrawData = ImGui::GetDrawData();

    if ((imDrawData->TotalVtxCount == 0) || (imDrawData->TotalIdxCount == 0)) {
        return;
    }

    ImDrawVert *vtxDst = new ImDrawVert[imDrawData->TotalVtxCount];
    ImDrawIdx *idxDst = new ImDrawIdx[imDrawData->TotalIdxCount];

    ImDrawVert *vtxOffset = vtxDst;
    ImDrawIdx *idxOffset = idxDst;


    for (int i = 0; i < imDrawData->CmdListsCount; i++) {
        const ImDrawList *cmdList = imDrawData->CmdLists[i];
        memcpy(vtxOffset, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxOffset, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxOffset += cmdList->VtxBuffer.Size;
        idxOffset += cmdList->IdxBuffer.Size;
    }

    m_renderingContext->vertex_buffer_flush(m_vertexBuffer.lock(), vtxDst, imDrawData->TotalVtxCount);
    m_renderingContext->index_buffer_flush(m_indexBuffer.lock(), idxDst, imDrawData->TotalIdxCount);

    delete[] vtxDst;
    delete[] idxDst;
}

void EditorLayer::handle_event(Event &e) {
    m_dispatcher.dispatch(e);
}

bool EditorLayer::handle_window_resized(WindowResizedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(e.get_width(), e.get_height());
    return false;
}

bool EditorLayer::handle_mouse_moved(MouseMoveEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(e.get_x(), e.get_y());
    return false;
}

bool EditorLayer::handle_mouse_button_pressed(MousePressedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[e.get_key()] = true;
    return false;
}

bool EditorLayer::handle_mouse_button_released(MouseReleasedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[e.get_key()] = false;
    return false;
}

bool EditorLayer::handle_mouse_scrolled(MouseScrolledEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheel = e.get_y();
    io.MouseWheelH = e.get_x();
    return false;
}

bool EditorLayer::handle_key(KeyEvent &e) {
    ImGuiIO &io = ImGui::GetIO();

    io.KeysDown[e.get_key()] = e.get_action() == EG_PRESS;

    // Modifiers are not reliable across m_systems
    io.KeyCtrl = io.KeysDown[EG_KEY_LEFT_CONTROL] || io.KeysDown[EG_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[EG_KEY_LEFT_SHIFT] || io.KeysDown[EG_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[EG_KEY_LEFT_ALT] || io.KeysDown[EG_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[EG_KEY_LEFT_SUPER] || io.KeysDown[EG_KEY_RIGHT_SUPER];
    return false;
}

bool EditorLayer::handle_key_typed(KeyTypedEvent &e) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(e.get_key());
    return false;
}

void EditorLayer::update_mouse_cursor() {
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

    EG_CURSOR cursorType;
    switch (imgui_cursor) {
        case ImGuiMouseCursor_TextInput:
            cursorType = EG_CURSOR::TEXT;
            break;
        case ImGuiMouseCursor_ResizeNS:
            cursorType = EG_CURSOR::VERT_RESIZE;
            break;
        case ImGuiMouseCursor_ResizeEW:
            cursorType = EG_CURSOR::HORI_RESIZE;
            break;
        case ImGuiMouseCursor_Hand:
            cursorType = EG_CURSOR::HAND;
            break;
        default:
            cursorType = EG_CURSOR::ARROW;
            break;
    }
    Application::instance().get_window()->set_cursor_shape(cursorType);

}

EG_ENGINE_END
