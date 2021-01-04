//
// Created by Ricardo on 04/01/21.
//

#include "TriangleApp.h"
#include "TriangleLayer.h"

TriangleApp::TriangleApp() :
    Eagle::Application("Triangle app", std::make_shared<Eagle::WindowGLFW>(1280, 720)) {
    m_layerStack.emplace_back(std::make_shared<TriangleLayer>());
}

