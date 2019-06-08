#include <eagle/Eagle.h>

class ExampleLayer : public Eagle::Layer {

public:
    ExampleLayer() = default;
    ~ExampleLayer() = default;

    std::weak_ptr<Eagle::Shader> m_shader;
    std::weak_ptr<Eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<Eagle::IndexBuffer> m_indexBuffer;

    virtual void handle_attach() override {
        EG_TRACE("Example layer attached!");

        m_shader = Eagle::RenderingContext::create_shader("shaders/shader_vert.spv", "shaders/shader_frag.spv");
        std::vector<float> vertices = {
                -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,
                -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.0f, 1.0f, 1.0f
        };
        m_vertexBuffer = Eagle::RenderingContext::create_vertex_buffer(vertices, 5);

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3
        };
        m_indexBuffer = Eagle::RenderingContext::create_index_buffer(indices);

    }

    virtual void handle_update() override {

    }

    virtual void handle_draw() override {
        Eagle::RenderingContext::bind_shader(m_shader.lock());
        Eagle::RenderingContext::draw_indexed_vertex_buffer(m_vertexBuffer.lock(), m_indexBuffer.lock());
    }

    virtual void handle_deattach() override{
        EG_TRACE("Example layer deattached!");
    }

    virtual void handle_event(Eagle::Event& e) override {
        EG_TRACE("Example layer event received!");
    }

};

class ExampleOverlay : public Eagle::Layer {

    std::weak_ptr<Eagle::Shader> m_shader;
    std::weak_ptr<Eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<Eagle::IndexBuffer> m_indexBuffer;

    virtual void handle_attach() override {
        EG_TRACE("Example layer attached!");

        m_shader = Eagle::RenderingContext::create_shader("shaders/shader_vert.spv", "shaders/shader_frag.spv");
        std::vector<float> vertices = {
                -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
                -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.0f, -1.0f, 0.0f, 1.0f, 1.0f
        };
        m_vertexBuffer = Eagle::RenderingContext::create_vertex_buffer(vertices, 5);

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3
        };
        m_indexBuffer = Eagle::RenderingContext::create_index_buffer(indices);
    }

    virtual void handle_draw() override {
        Eagle::RenderingContext::bind_shader(m_shader.lock());
        Eagle::RenderingContext::draw_indexed_vertex_buffer(m_vertexBuffer.lock(), m_indexBuffer.lock());
    }



};


Eagle::ApplicationCreateInfo Eagle::create_application_info() {
    Eagle::ApplicationCreateInfo config = {};
    config.appName = "Dummy";
    config.windowType = new Eagle::WindowGLFW(new Eagle::VulkanContext(), 1280, 720);
    config.layers.push_back(std::make_shared<ExampleLayer>());
    config.layers.push_back(std::make_shared<ExampleOverlay>());
    config.coreLogLevel = Eagle::Log::TRACE;
    config.clientLogLevel = Eagle::Log::INFO;
    return config;
}