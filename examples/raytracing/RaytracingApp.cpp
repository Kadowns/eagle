//
// Created by Ricardo on 04/01/21.
//

#include <eagle/engine/SceneLayer.h>
#include <eagle/editor/EditorLayer.h>
#include <eagle/engine/renderer/RenderLayer.h>
#include "RaytracingApp.h"

RaytracingApp::RaytracingApp() :
    Eagle::Application("Raytracing", std::make_shared<Eagle::WindowGLFW>(1280, 720)) {
    m_layerStack.emplace_back(std::make_shared<Eagle::Engine::SceneLayer>());
    m_layerStack.emplace_back(std::make_shared<Eagle::Editor::EditorLayer>());
    m_layerStack.emplace_back(std::make_shared<Eagle::Engine::RenderLayer>());
    m_layerStack.emplace_back(std::make_shared<Eagle::InputLayer>());
}
