//
// Created by Novak on 06/08/2019.
//

#include "eagle/engine/editor/SceneWindow.h"
#include <imgui.h>

EG_ENGINE_BEGIN

SceneWindow::SceneWindow() :
    EditorWindow("Scene"){

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
    m_scene->init(m_camera);

}

SceneWindow::~SceneWindow() {

}

void SceneWindow::handle_update() {

    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    ImVec2 region(vMax.x - vMin.x, vMax.y - vMin.y);

    ImGui::Image((ImTextureID)&m_descriptor, region);
}

void SceneWindow::draw() {
    m_scene->render(m_camera);
}


EG_ENGINE_END



