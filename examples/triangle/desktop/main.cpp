//
// Created by Ricardo on 11/28/2020.
//

#include <triangle_application.h>

#include <eagle/platform/desktop/desktop_application.h>

int main(){

    eagle::ApplicationCreateInfo applicationCreateInfo = {};
    applicationCreateInfo.applicationName = "Triangle";
    applicationCreateInfo.engineName = "Eagle";
    applicationCreateInfo.delegate = new TriangleApplication();

    eagle::DesktopApplicationCreateInfo desktopApplicationCreateInfo = {};
    desktopApplicationCreateInfo.width = 1280;
    desktopApplicationCreateInfo.height = 720;

    eagle::DesktopApplication application(applicationCreateInfo, desktopApplicationCreateInfo);

    try {
        application.run();
    }
    catch(const std::exception& e) {
        EG_CRITICAL("eagle", "An exception occurred: {0}", e.what());
        return 1;
    }

    return 0;
}