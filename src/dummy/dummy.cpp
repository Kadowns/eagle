#include <eagle/Eagle.h>

class ExampleLayer : public Eagle::Layer {

public:
    ExampleLayer() = default;
    ~ExampleLayer() = default;

    virtual void handle_attach() override {
        EG_TRACE("Example layer attached!");

        Eagle::RenderingContext::create_shader("shaders/shader_vert.spv", "shaders/shader_frag.spv");
        Eagle::RenderingContext::create_shader("shaders/shader_two_vert.spv", "shaders/shader_frag.spv");

    }

    virtual void handle_update() override {
        EG_TRACE("Example layer updated!");
    }

    virtual void handle_deattach() override{
        EG_TRACE("Example layer deattached!");
    }

    virtual void handle_event(Eagle::Event& e) override {
        EG_TRACE("Example layer event received!");
    }

};


Eagle::ApplicationCreateInfo Eagle::create_application_info() {
    Eagle::ApplicationCreateInfo config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);
    config.layers.push_back(std::make_shared<ExampleLayer>());
    config.coreLogLevel = Eagle::Log::TRACE;
    config.clientLogLevel = Eagle::Log::INFO;
    return config;
}