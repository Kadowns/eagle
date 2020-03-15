//
// Created by Novak on 16/07/2019.
//
#include <eagle/editor/EditorLayer.h>
#include <eagle/editor/windows/InspectorWindow.h>
#include <eagle/editor/windows/HierarchyWindow.h>
#include <eagle/editor/windows/ResourcesWindow.h>
#include <eagle/editor/windows/SceneWindow.h>

EG_EDITOR_BEGIN

void EditorLayer::handle_attach() {

    m_dispatcher.add_listener<WindowCloseEvent>([&](Event& e){
        Application::instance().quit();
        return true;
    });

    m_dispatcher.add_listener<MouseMoveEvent>([&](Event &e) {
        return handle_mouse_moved(*(MouseMoveEvent*) &e);
    });
    m_dispatcher.add_listener<MouseButtonEvent>([&](Event &e) {
        return handle_mouse_button(*(MouseButtonEvent*) &e);
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
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.46f, 0.4f, 0.46f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.16f, 0.15f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.3f, 0.32f, 0.3f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.125f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.12f, 0.125f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.215f, 0.215f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.2f, 0.215f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.4f, 0.475f, 0.45f, 0.8f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.65f, 0.6f, 0.65f, 0.8f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.5f, 0.55f, 0.8f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4f, 0.45f, 0.6f, 0.8f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.55f, 0.55f, 0.7f, 0.8f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.35f, 0.35f, 0.365f, 0.8f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.46f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.45f, 0.4f, 0.425f, 0.8f);
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.GrabRounding = 2;
    style.PopupRounding = 1;
    style.WindowRounding = 2;
    style.WindowPadding = ImVec2(2, 2);

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


    io.DisplaySize = ImVec2(Application::instance().window().get_width(), Application::instance().window().get_height());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows



    
    Pixel *pixels;
    Texture2DCreateInfo fontCreateInfo = {};
    io.Fonts->GetTexDataAsRGBA32(&pixels, &fontCreateInfo.width, &fontCreateInfo.height);
    fontCreateInfo.format = Format::R8G8B8A8_UNORM;
    fontCreateInfo.filter = Filter::LINEAR;
    fontCreateInfo.layerCount = 1;
    fontCreateInfo.mipLevels = 1;
    fontCreateInfo.channels = 4;
    fontCreateInfo.pixels = std::vector<Pixel>(pixels,
                                                      pixels + fontCreateInfo.width * fontCreateInfo.height * 4);
    
    
    RenderingContext& context = Renderer::instance().context();

    m_font = context.create_texture_2d(fontCreateInfo);


    VertexLayout vertexLayout = VertexLayout(5, {
            Format::R32G32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R8G8B8A8_UNORM
    });


    DescriptorBindingDescription binding = {};
    binding.shaderStage = ShaderStage::FRAGMENT;
    binding.descriptorType = DescriptorType::IMAGE_2D;
    binding.binding = 0;

    m_descriptorLayout = context.create_descriptor_set_layout({binding});


    ShaderPipelineInfo pipelineInfo = ShaderPipelineInfo(vertexLayout);
    pipelineInfo.blendEnable = true;
    pipelineInfo.dynamicStates = true;
    pipelineInfo.depthTesting = false;
    pipelineInfo.offscreenRendering = false;

    m_shader = context.create_shader({
        {ShaderStage::VERTEX, ProjectRoot + "/shaders/text.vert"},
        {ShaderStage::FRAGMENT, ProjectRoot + "/shaders/text.frag"},
    }, pipelineInfo);

    m_descriptor = context.create_descriptor_set(m_descriptorLayout.lock(), {m_font.lock()->get_image().lock()});

    io.Fonts->TexID = (ImTextureID)&m_descriptor;

    m_vertexBuffer = context.create_vertex_buffer(nullptr, 0, vertexLayout,
                                                                   BufferUsage::DYNAMIC);
    m_indexBuffer = context.create_index_buffer(nullptr, 0, IndexBufferType::UINT_16,
                                                                 BufferUsage::DYNAMIC);

    m_editors.push_back(std::make_shared<InspectorWindow>());
    m_editors.push_back(std::make_shared<SceneWindow>());
    m_editors.push_back(std::make_shared<HierarchyWindow>());
    m_editors.push_back(std::make_shared<ResourcesWindow>());

}

void EditorLayer::handle_update() {

    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    ImGui::NewFrame();
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = time + 0.0001f;

    //ImGui::ShowDemoWindow();
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

    //process even if the dockspace is collapsed, so child windows will not lose their parents
    ImGui::Begin("Dock", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    if (ImGui::BeginMenuBar()){

        if (ImGui::BeginMenu("File")){
            ImGui::MenuItem("New project");
            ImGui::MenuItem("Open project");
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }


    // DockSpace
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
    draw();
}

void EditorLayer::draw() {

    RenderingContext& context = Renderer::instance().context();

    if (!context.prepare_frame()){
        EG_WARNING("Skipped frame rendering!");
        return;
    }

    auto commandBuffer = context.create_command_buffer();

    for (auto& editor : m_editors){
        editor->draw(commandBuffer);
    }

    ImGuiIO &io = ImGui::GetIO();
    ImDrawData *imDrawData = ImGui::GetDrawData();

    commandBuffer->begin_render_pass(context.main_render_target());
    // Render commands
    if (imDrawData->CmdListsCount > 0) {
        commandBuffer->bind_shader(m_shader.lock());

        commandBuffer->set_viewport(io.DisplaySize.x, io.DisplaySize.y, 0, 0, 0, 1);

        pushConstBlock.scale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
        pushConstBlock.translate = glm::vec2(-1.0f);

        commandBuffer->push_constants(m_shader.lock(), ShaderStage::VERTEX, 0,
                                      sizeof(pushConstBlock), &pushConstBlock);

        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;

        commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
        commandBuffer->bind_index_buffer(m_indexBuffer.lock());

        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++) {

            const ImDrawList *cmdList = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmdList->CmdBuffer.Size; j++) {

                const ImDrawCmd *cmd = &cmdList->CmdBuffer[j];
                uint32_t x = std::max((int32_t) (cmd->ClipRect.x), 0);
                uint32_t y = std::max((int32_t) (cmd->ClipRect.y), 0);
                uint32_t w = (uint32_t) (cmd->ClipRect.z - cmd->ClipRect.x);
                uint32_t h = (uint32_t) (cmd->ClipRect.w - cmd->ClipRect.y);
                commandBuffer->set_scissor(w, h, x, y);

                commandBuffer->bind_descriptor_sets(m_shader.lock(), static_cast<Handle<DescriptorSet>*>(cmd->TextureId)->lock(), 0);

                commandBuffer->draw_indexed(cmd->ElemCount, indexOffset, vertexOffset);

                indexOffset += cmd->ElemCount;
            }
            vertexOffset += cmdList->VtxBuffer.Size;
        }
    }
    commandBuffer->end_render_pass();
    commandBuffer->finish();
    context.submit_command_buffer(commandBuffer);
    context.present_frame();

}

void EditorLayer::handle_deattach() {
    ImGui::DestroyContext();
    Renderer::terminate();
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

    m_vertexBuffer.lock()->upload(vtxDst, imDrawData->TotalVtxCount);
    m_indexBuffer.lock()->upload(idxDst, imDrawData->TotalIdxCount);

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

bool EditorLayer::handle_mouse_button(MouseButtonEvent &e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.get_key()] = e.get_action() == EG_PRESS || e.get_action() == EG_REPEAT;
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

    io.KeysDown[e.get_key()] = e.get_action() == EG_PRESS || e.get_action() == EG_REPEAT;

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

    Cursor cursorType;
    switch (imgui_cursor) {
        case ImGuiMouseCursor_TextInput:
            cursorType = Cursor::TEXT;
            break;
        case ImGuiMouseCursor_ResizeNS:
            cursorType = Cursor::VERT_RESIZE;
            break;
        case ImGuiMouseCursor_ResizeEW:
            cursorType = Cursor::HORI_RESIZE;
            break;
        case ImGuiMouseCursor_Hand:
            cursorType = Cursor::HAND;
            break;
        default:
            cursorType = Cursor::ARROW;
            break;
    }
    Application::instance().window().set_cursor_shape(cursorType);

}

EG_EDITOR_END
