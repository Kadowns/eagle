//
// Created by Novak on 06/08/2019.
//

#include "eagle/editor/SceneWindow.h"
#include <imgui/imgui.h>
#include <eagle/editor/Serializer.h>
#include <eagle/engine/resources/ResourcesPool.h>

EG_ENGINE_BEGIN

SceneWindow::SceneWindow() :
    EditorWindow("Scene"),
    m_cameraTransform(glm::vec3(0.0f, 1.0f, 30.0f), glm::quat(glm::vec3(0)), glm::vec3(1)){

    m_camera = std::make_shared<Camera>(60.0f, 720.0f, 480.0f, 0.1f, 1000.0f,
                                        glm::vec3(0.0f, 1.0f, 30.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));



    auto renderingContext = Application::instance().get_window()->get_rendering_context();

    DescriptorBinding binding = {};
    binding.binding = 0;
    binding.shaderStage = EG_SHADER_STAGE::FRAGMENT;
    binding.descriptorType = EG_DESCRIPTOR_TYPE::IMAGE_2D;


    m_descriptorLayout = renderingContext->create_descriptor_set_layout({binding});

    m_descriptor = renderingContext->create_descriptor_set(m_descriptorLayout.lock(),
                                                                  {m_camera->render_target()->get_image(0).lock()});

    m_scene = std::make_shared<Scene>();
    m_scene->init();

    //ResourceManager::initialize(ProjectRoot);
}

SceneWindow::~SceneWindow() {

}

void SceneWindow::handle_update() {

    static auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    ImGuiIO& io = ImGui::GetIO();


    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    ImVec2 region(vMax.x - vMin.x, vMax.y - vMin.y);
    ImGui::Image((ImTextureID)&m_descriptor, region);

    static ImVec2 windowSize = ImVec2(0, 0);

    if (windowSize.x != region.x || windowSize.y != region.y){
        m_camera->set_projection(60.0f, region.x, region.y, 0.1f, 1000.0f);
        windowSize = region;
    }

    static float speed = 40.0f;
    if (ImGui::IsWindowFocused()){
        glm::vec3 displacement = glm::vec3(0);
        if (Input::instance().key_down(EG_KEY_W)){
            displacement += m_cameraTransform.front();
        }
        if (Input::instance().key_down(EG_KEY_S)){
            displacement += -m_cameraTransform.front();
        }
        if (Input::instance().key_down(EG_KEY_D)){
            displacement += m_cameraTransform.right();
        }
        if (Input::instance().key_down(EG_KEY_A)){
            displacement += -m_cameraTransform.right();
        }



        speed += Input::instance().mouse_scroll_delta().second;

        m_cameraTransform.position +=  time * speed * displacement;

        if (Input::instance().mouse_button_down(EG_MOUSE_BUTTON_RIGHT)){
            io.MousePos.x = region.x / 2;
            io.MousePos.y = region.y / 2;
            Application::instance().get_window()->set_cursor_visible(false);

            auto mousePosition = Input::instance().mouse_move_delta();
            glm::vec2 mouseDelta = glm::vec2(mousePosition.first, mousePosition.second) * time * 80.0f;

            m_cameraTransform.rotation *= glm::quat(glm::radians(glm::vec3(mouseDelta.y, -mouseDelta.x, 0)));
            glm::normalize(m_cameraTransform.rotation);
        }else{
            Application::instance().get_window()->set_cursor_visible(true);
        }

        m_camera->set_view(m_cameraTransform.position, m_cameraTransform.position + m_cameraTransform.front(), m_cameraTransform.up());
    }

    ImVec2 curPos = ImGui::GetWindowPos();

    ImVec2 pos(curPos.x + vMin.x, curPos.y + vMin.y);
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    ImGui::SetNextWindowPos(pos);
    if (ImGui::Begin("Scene status", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration |
                                              ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
                                              ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {

        ImGui::Text("Delta time: %f", time);
        ImGui::Text("FPS: %f", 1 / time);
        auto position = m_cameraTransform.position;
        ImGui::Text("Camera position: %f, %f, %f", position.x, position.y, position.z);
        ImGui::Text("Camera speed: %f", speed);

        if (ImGui::Button("Save scene")){
            Serializer serializer(ProjectRoot + "/scene.txt");
            serializer.save(*m_scene);
        }

    }
    ImGui::End();
}

void SceneWindow::draw() {
    m_scene->render(m_camera);
}


EG_ENGINE_END



