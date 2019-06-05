#include <eagle/core/Application.h>
#include <eagle/renderer/vulkan/VulkanContext.h>
#include <eagle/core/WindowGLFW.h>
#include <iostream>

int main() {

    Eagle::AppConfig config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);

    Eagle::Application app(config);
    try{
        app.run();
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}