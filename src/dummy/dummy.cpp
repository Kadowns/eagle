//
// Created by Novak on 18/06/2019.
//
#include <eagle/Eagle.h>

#include "OverlayTest.h"
#include "SceneTest.h"

Eagle::ApplicationCreateInfo create_application_info() {
    Eagle::ApplicationCreateInfo config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);
    config.layers.push_back(std::make_shared<OverlayTest>());
    config.layers.push_back(std::make_shared<SceneTest>());
    config.coreLogLevel = Eagle::Log::WARN;
    config.clientLogLevel = Eagle::Log::INFO;
    return config;
}

int main(){

    Eagle::ApplicationCreateInfo createInfo = create_application_info();
    std::unique_ptr<Eagle::Application> app = std::make_unique<Eagle::Application>(createInfo);

    try{
        app->run();
    }catch(std::exception& e){
        EG_FATAL_F("Exception caught! Message:{0}", e.what());
        return 1;
    }

    return 0;
}
