#include <eagle/core/Application.h>
#include <eagle/renderer/vulkan/VulkanContext.h>
#include <eagle/core/DefaultWindow.h>

int main() {

    Eagle::AppConfig config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::DefaultWindow(new Eagle::VulkanContext(1280, 720));

    Eagle::Application app(config);
    app.run();

    return 0;
}