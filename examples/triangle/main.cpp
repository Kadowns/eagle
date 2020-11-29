//
// Created by Ricardo on 11/28/2020.
//

#include "TriangleLayer.h"

int main(){

    Eagle::ApplicationCreateInfo createInfo = {};
    createInfo.appName = "Eagle triangle";
    createInfo.clientLogLevel = Eagle::Log::TRACE;
    createInfo.coreLogLevel = Eagle::Log::TRACE;
    createInfo.windowType = new Eagle::WindowGLFW(1280, 720);
    createInfo.layers.emplace_back(std::make_shared<TriangleLayer>());

    Eagle::Application app(createInfo);

    try {
        app.run();
    } catch(const std::exception& e) {
        EG_FATAL_F("An exception occurred: {0}", e.what());
        return 1;
    }

    return 0;
}