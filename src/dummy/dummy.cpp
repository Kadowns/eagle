#include <eagle/Eagle.h>

#include "OverlayTest.h"
#include "SceneTest.h"

Eagle::ApplicationCreateInfo Eagle::create_application_info() {
    Eagle::ApplicationCreateInfo config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);
    config.layers.push_back(std::make_shared<OverlayTest>());
    config.layers.push_back(std::make_shared<SceneTest>());
    config.coreLogLevel = Eagle::Log::TRACE;
    config.clientLogLevel = Eagle::Log::INFO;
    return config;
}