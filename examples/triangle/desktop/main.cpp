//
// Created by Ricardo on 11/28/2020.
//

#include <TriangleApp.h>

#include <eagle/platform/desktop/DesktopApplication.h>

int main(){

    eagle::Log::init(eagle::Log::TRACE, eagle::Log::TRACE);

    eagle::DesktopApplication application(1280, 720, new TriangleApp());

    try {
        application.run();
    } catch(const std::exception& e) {
        EG_FATAL_F("An exception occurred: {0}", e.what());
        return 1;
    }

    return 0;
}