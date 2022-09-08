//
// Created by Ricardo on 2/15/2021.
//

#include "application.h"

eagle::Application* eagle::Application::s_instance = nullptr;

eagle::Application::Application(const eagle::ApplicationCreateInfo& createInfo) : m_createInfo(createInfo){

}

eagle::Application::~Application() = default;
