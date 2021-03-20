//
// Created by Ricardo on 11/28/2020.
//

#include <triangle_application.h>

#include <eagle/platform/desktop/desktop_application.h>

int main(){


    eagle::DesktopApplication application(1280, 720, new TriangleApplication());

    try {
        application.run();
    } catch(const std::exception& e) {
        EG_CRITICAL("eagle", "An exception occurred: {0}", e.what());
        return 1;
    }

    return 0;
}