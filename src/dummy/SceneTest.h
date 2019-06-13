//
// Created by Novak on 12/06/2019.
//

#ifndef EAGLE_SCENETEST_H
#define EAGLE_SCENETEST_H

#include <eagle/Eagle.h>

class SceneTest : public Eagle::Layer {

public:

    std::weak_ptr<Eagle::Shader> m_shader;
    std::weak_ptr<Eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<Eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<Eagle::UniformBuffer> m_uniformBuffer;
    std::weak_ptr<Eagle::DescriptorSet> m_descriptorSet;

    glm::mat4 view, proj, model;

    glm::quat rotation;

    virtual void handle_attach() override {
        EG_TRACE("Example layer attached!");

        rotation = glm::quat(glm::vec3(0));

        m_shader = Eagle::RenderingContext::create_shader("shaders/shader_vert.spv", "shaders/shader_frag.spv");
        std::vector<float> vertices = {
                1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5, // 0
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5,  // 1
                -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5,  // 2
                -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.5,  // 3

                1.0f, 1.0f, -1.0f, 1.0f, 0.5, 1.0f, // 4
                1.0f, -1.0f, -1.0f, 1.0f, 0.5, 1.0f,  // 5
                -1.0f, -1.0f, -1.0f, 1.0f, 0.5, 1.0f,  // 6
                -1.0f, 1.0f, -1.0f, 1.0f, 0.5, 1.0f,  // 7

                -1.0f, 1.0f, 1.0f, 1.0f, 0.5, 0.5, // 8
                -1.0f, 1.0f, -1.0f, 1.0f, 0.5, 0.5,  // 9
                -1.0f, -1.0f, -1.0f, 1.0f, 0.5, 0.5,  // 10
                -1.0f, -1.0f, 1.0f, 1.0f, 0.5, 0.5,  // 11

                1.0f, -1.0f, 1.0f, 0.5, 0.5, 1.0f, // 12
                1.0f, -1.0f, -1.0f, 0.5, 0.5, 1.0f,  // 13
                1.0f, 1.0f, -1.0f, 0.5, 0.5, 1.0f,  // 14
                1.0f, 1.0f, 1.0f, 0.5, 0.5, 1.0f,  // 15

                1.0f, -1.0f, -1.0f, 0.5, 1.0f, 1.0f, // 16
                1.0f, -1.0f, 1.0f, 0.5, 1.0f, 1.0f,  // 17
                -1.0f, -1.0f, 1.0f, 0.5, 1.0f, 1.0f,  // 18
                -1.0f, -1.0f, -1.0f, 0.5, 1.0f, 1.0f,  // 19

                1.0f, 1.0f, 1.0f, 0.5, 1.0f, 0.5, // 20
                1.0f, 1.0f, -1.0f, 0.5, 1.0f, 0.5,  // 21
                -1.0f, 1.0f, -1.0f, 0.5, 1.0f, 0.5,  // 22
                -1.0f, 1.0f, 1.0f, 0.5, 1.0f, 0.5,  // 23

        };
        m_vertexBuffer = Eagle::RenderingContext::create_vertex_buffer(vertices, 6);

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23
        };
        m_indexBuffer = Eagle::RenderingContext::create_index_buffer(indices);

        m_uniformBuffer = Eagle::RenderingContext::create_uniform_buffer(m_shader.lock()->get_shader_item("mvp"));

        m_descriptorSet = Eagle::RenderingContext::create_descriptor_set(m_shader.lock(), {m_uniformBuffer.lock()});

        view = glm::lookAt(glm::vec3(0.0f, 3.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        proj = glm::perspective(glm::radians(45.0f), Eagle::Application::get_window_aspect(), 0.1f, 10.0f);
        proj[1][1] *= -1;
    }

    virtual void handle_update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        rotation = glm::quat(glm::radians(glm::vec3(25.0f, 15.0f, 7.5f) * time));
        glm::normalize(rotation);

        model = glm::mat4(1) * glm::mat4_cast(rotation);

    }

    virtual void handle_draw() override {

        glm::mat4 mvp = proj * view * model;
        Eagle::RenderingContext::flush_uniform_buffer_data(m_uniformBuffer.lock(), &mvp);
        Eagle::RenderingContext::bind_shader(m_shader.lock());
        Eagle::RenderingContext::bind_descriptor_set(m_descriptorSet.lock());
        Eagle::RenderingContext::draw_indexed_vertex_buffer(m_vertexBuffer.lock(), m_indexBuffer.lock());
    }

    virtual void handle_deattach() override{
        EG_TRACE("Example layer deattached!");
    }

    virtual void handle_event(Eagle::Event& e) override {
        EG_TRACE("Example layer event received!");
        Eagle::EventDispatcher dispatcher(e);
        dispatcher.dispatch<Eagle::WindowResizedEvent>(BIND_EVENT_FN(SceneTest::handle_window_resized));
    }

    bool handle_window_resized(Eagle::WindowResizedEvent& e){
        proj = glm::perspective(glm::radians(45.0f), (float)e.get_width() / e.get_height(), 0.1f, 10.0f);
        proj[1][1] *= -1;
        return true;
    }

};

#endif //EAGLE_SCENETEST_H
