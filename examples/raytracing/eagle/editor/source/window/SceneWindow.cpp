//
// Created by Novak on 06/04/2020.
//

#include <eagle/editor/window/SceneWindow.h>

EG_EDITOR_BEGIN

SceneWindow::SceneWindow() : EditorWindow("SceneLayer") {
    raytracer_target_created_callback = [&](const Engine::OnRaytracerTargetCreated& ev){
        handle_raytracer_target_created(ev.target);
    };
    Engine::EventMaster::instance().subscribe<Engine::OnRaytracerTargetCreated>(&raytracer_target_created_callback);



}

SceneWindow::~SceneWindow() {
    Engine::EventMaster::instance().unsubscribe<Engine::OnRaytracerTargetCreated>(&raytracer_target_created_callback);
}

void SceneWindow::handle_window_update() {
    static bool configured = false;
    if (!configured){
        m_scene.systems.configure();
        configured = true;
    }

    m_scene.systems.update_all(Time::delta_time());
    if (!m_descriptorSet.lock()){
        ImGui::Text("No target image set.");
        return;
    }

    ImGui::GetWindowDrawList()->AddCallback(DisableBlending, nullptr);

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    ImVec2 region(vMax.x - vMin.x, vMax.y - vMin.y);

    ImGui::Image((ImTextureID) &m_descriptorSet, region);
    ImGui::GetWindowDrawList()->AddCallback(EnableBlending, nullptr);
}

void SceneWindow::handle_raytracer_target_created(const Reference<Image> &target) {
    if (!target){
        return;
    }

    if (m_descriptorSet.lock()){
        m_descriptorSet.lock()->update({target});
    }
    else {
        DescriptorBindingDescription bindingDescription = {};
        bindingDescription.name = "uTexture";
        bindingDescription.shaderStage = ShaderStage::FRAGMENT;
        bindingDescription.descriptorType = DescriptorType::SAMPLED_IMAGE;
        bindingDescription.binding = 0;
        auto layout = Engine::RenderMaster::context().create_descriptor_set_layout({bindingDescription});
        m_descriptorSet = Engine::RenderMaster::context().create_descriptor_set(layout.lock(), {target});
    }
}




EG_EDITOR_END


