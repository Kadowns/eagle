//
// Created by Ricardo on 11/28/2020.
//

#include <compute_application.h>

#include <eagle/platform/desktop/desktop_application.h>

int main(){

    eagle::ApplicationCreateInfo applicationCreateInfo = {};
    applicationCreateInfo.applicationName = "Compute";
    applicationCreateInfo.engineName = "Eagle";
    applicationCreateInfo.delegate = new ComputeApplication();

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