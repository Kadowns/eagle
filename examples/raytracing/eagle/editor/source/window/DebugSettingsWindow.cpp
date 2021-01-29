//
// Created by Novak on 27/05/2020.
//

#include <eagle/editor/window/DebugSettingsWindow.h>

using namespace Eagle::Engine;

EG_EDITOR_BEGIN

DebugSettingsWindow::DebugSettingsWindow() :
    EditorWindow("Physics Settings"){
    m_timer = &Application::instance().timer();
}

DebugSettingsWindow::~DebugSettingsWindow() {

}

void DebugSettingsWindow::handle_window_update() {

    auto& settings = SingletonComponent::get<PhysicsSettings>();
    auto& sceneData = SingletonComponent::get<SceneData>();
    auto& spawner = SingletonComponent::get<Spawner>();
    auto& scene = SceneManager::current_scene();

    ImGui::SliderFloat3("Gravity", &settings.gravity[0], -50, 50);
    float timeScale = m_timer->time_scale();
    if (ImGui::SliderFloat("Time Step", &timeScale, 0.0f, 1.0f)){
        m_timer->set_time_scale(timeScale);
    }

    ImGui::InputFloat3("Cube rotation:", &sceneData.cubeRotation[0]);

    if (ImGui::Button("Reset scene")){
        Application::instance().event_bus().emit(OnSceneRecreate{});
    }

    ImGui::Checkbox("Static spawn", &spawner.isStatic);
    ImGui::InputFloat3("Spawner rotation", &spawner.rotation[0]);
    ImGui::InputFloat3("Spawner radius", &spawner.radius[0]);
}

EG_EDITOR_END



