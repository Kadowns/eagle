//
// Created by Novak on 12/06/2019.
//

#ifndef EAGLE_SCENETEST_H
#define EAGLE_SCENETEST_H

#include <eagle/Eagle.h>
#include "DummyEvents.h"

class SceneTest : public Eagle::Layer {

public:

    struct Light {
        glm::vec4 direction, color;
        std::weak_ptr<Eagle::UniformBuffer> buffer;


        void update(){
            Eagle::RenderingContext::uniform_buffer_flush(buffer.lock(), &direction);
        }

    } m_light;

    struct {
        std::weak_ptr<Eagle::Shader> shader;
        std::weak_ptr<Eagle::VertexBuffer> vertices;
        std::weak_ptr<Eagle::IndexBuffer> indices;
        std::weak_ptr<Eagle::UniformBuffer> uniform;
        std::weak_ptr<Eagle::DescriptorSet> descriptorSet;

        struct{
            alignas(4)  float amount;
            alignas(16) glm::mat3 kernel;
        }ubo;

    } postFx;

    std::weak_ptr<Eagle::Shader> m_shader;
    std::weak_ptr<Eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<Eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<Eagle::UniformBuffer> m_mvpBuffer, m_colorBuffer;
    std::weak_ptr<Eagle::DescriptorSet> m_descriptorSet;
    std::weak_ptr<Eagle::Texture2D> m_texture;
    std::weak_ptr<Eagle::RenderTarget> m_renderTarget;

    float rotationDirection = 1;

    glm::mat4 view, proj, model, mvp;
    glm::vec4 color;

    glm::quat rotation;

    virtual void handle_attach() override {

        m_renderTarget = Eagle::RenderingContext::create_render_target();

        postFx.shader = Eagle::RenderingContext::create_shader("shaders/postprocess.vert", "shaders/postprocess.frag");
        postFx.uniform = Eagle::RenderingContext::create_uniform_buffer(sizeof(postFx.ubo));
        postFx.descriptorSet = Eagle::RenderingContext::create_descriptor_set(
                postFx.shader.lock(),
                {postFx.uniform.lock()},
                {m_renderTarget.lock()->get_image().lock()}
                );

        std::vector<float> vertices = {
                -1.0f, -1.0f, 0.0f, 0.0f,
                -1.0f, 1.0f,  0.0f, 1.0f,
                1.0f, 1.0f,   1.0f, 1.0f,
                1.0f, -1.0f,  1.0f, 0.0f
        };
        postFx.vertices = Eagle::RenderingContext::create_vertex_buffer(vertices, 4);

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3
        };

        postFx.indices = Eagle::RenderingContext::create_index_buffer(indices);

        postFx.ubo.kernel = glm::mat3(
                -1.0f, -2.0f, -1.0f,
                -1.0f, 0.0f, 1.0f,
                1.0f, 2.0f, 1.0f
        );
        postFx.ubo.amount = 0.1f;
        Eagle::RenderingContext::uniform_buffer_flush(postFx.uniform.lock(), &postFx.ubo);


        m_shader = Eagle::RenderingContext::create_shader("shaders/shader.vert", "shaders/shader.frag", m_renderTarget.lock());
        vertices = {
                1.0f, -1.0f, 1.0f,      0.0f, 0.0f,   0.0f, 0.0f, 1.0f,      // 0
                1.0f, 1.0f, 1.0f,       1.0f, 0.0f,   0.0f, 0.0f, 1.0f,      // 1
                -1.0f, 1.0f, 1.0f,      0.0f, 1.0f,   0.0f, 0.0f, 1.0f,      // 2
                -1.0f, -1.0f, 1.0f,     1.0f, 1.0f,   0.0f, 0.0f, 1.0f,      // 3

                1.0f, 1.0f, -1.0f,      0.0f, 0.0f,   0.0f, 0.0f, -1.0f,      // 4
                1.0f, -1.0f, -1.0f,     1.0f, 0.0f,   0.0f, 0.0f, -1.0f,      // 5
                -1.0f, -1.0f, -1.0f,    0.0f, 1.0f,   0.0f, 0.0f, -1.0f,      // 6
                -1.0f, 1.0f, -1.0f,     1.0f, 1.0f,   0.0f, 0.0f, -1.0f,      // 7

                -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,      // 8
                -1.0f, 1.0f, -1.0f,     1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,      // 9
                -1.0f, -1.0f, -1.0f,    0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,      // 10
                -1.0f, -1.0f, 1.0f,     1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,      // 11

                1.0f, -1.0f, 1.0f,      0.0f, 0.0f,   1.0f, 0.0f, 0.0f,      // 12
                1.0f, -1.0f, -1.0f,     1.0f, 0.0f,   1.0f, 0.0f, 0.0f,      // 13
                1.0f, 1.0f, -1.0f,      0.0f, 1.0f,   1.0f, 0.0f, 0.0f,      // 14
                1.0f, 1.0f, 1.0f,       1.0f, 1.0f,   1.0f, 0.0f, 0.0f,      // 15

                1.0f, -1.0f, -1.0f,     0.0f, 0.0f,   0.0f, -1.0f, 0.0f,     // 16
                1.0f, -1.0f, 1.0f,      1.0f, 0.0f,   0.0f, -1.0f, 0.0f,      // 17
                -1.0f, -1.0f, 1.0f,     0.0f, 1.0f,   0.0f, -1.0f, 0.0f,      // 18
                -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,   0.0f, -1.0f, 0.0f,     // 19

                1.0f, 1.0f, 1.0f,       0.0f, 0.0f,   0.0f, 1.0f, 0.0f,      // 20
                1.0f, 1.0f, -1.0f,      1.0f, 0.0f,   0.0f, 1.0f, 0.0f,      // 21
                -1.0f, 1.0f, -1.0f,     0.0f, 1.0f,   0.0f, 1.0f, 0.0f,      // 22
                -1.0f, 1.0f, 1.0f,      1.0f, 1.0f,   0.0f, 1.0f, 0.0f,      // 23

        };
        m_vertexBuffer = Eagle::RenderingContext::create_vertex_buffer(vertices, 8);

        indices = {
                0, 1, 2, 0, 2, 3,
                4, 5, 6, 4, 6, 7,
                8, 9, 10, 8, 10, 11,
                12, 13, 14, 12, 14, 15,
                16, 17, 18, 16, 18, 19,
                20, 21, 22, 20, 22, 23
        };
        m_indexBuffer = Eagle::RenderingContext::create_index_buffer(indices);

        m_mvpBuffer = Eagle::RenderingContext::create_uniform_buffer(sizeof(glm::mat4));
        m_colorBuffer = Eagle::RenderingContext::create_uniform_buffer(sizeof(glm::vec4));
        m_light.buffer = Eagle::RenderingContext::create_uniform_buffer(sizeof(glm::vec4) * 2);
        m_light.color = glm::vec4(1.0f);
        m_light.direction = glm::vec4(0.2f, -0.2f, 0.8f, 1.0f);
        m_light.update();

        m_texture = Eagle::RenderingContext::create_texture_2d("textures/box.png");
        m_descriptorSet = Eagle::RenderingContext::create_descriptor_set(m_shader.lock(),
                                                                         {m_mvpBuffer.lock(), m_colorBuffer.lock(), m_light.buffer.lock()},
                                                                         {m_texture.lock()->get_image().lock()});

        rotation = glm::quat(glm::vec3(0));

        view = glm::lookAt(glm::vec3(0.0f, 3.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        proj = glm::perspective(glm::radians(45.0f), Eagle::Application::get_window_aspect(), 0.1f, 10.0f);
        proj[1][1] *= -1;

        color = glm::vec4(1.0);
        Eagle::RenderingContext::uniform_buffer_flush(m_colorBuffer.lock(), &color);


    }

    virtual void handle_update() override {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        rotation = glm::quat(glm::radians(glm::vec3(25.0f, 15.0f, 7.5f) * time * rotationDirection));
        glm::normalize(rotation);

        model = glm::mat4(1) * glm::mat4_cast(rotation);
        mvp = proj * view * model;
        Eagle::RenderingContext::uniform_buffer_flush(m_mvpBuffer.lock(), &mvp);

        postFx.ubo.amount = abs(sin((double)time)) * 0.5f + 0.5f;
        Eagle::RenderingContext::uniform_buffer_flush(postFx.uniform.lock(), &postFx.ubo);


    }

    virtual void handle_draw() override {

        Eagle::RenderingContext::begin_draw_offscreen(m_renderTarget.lock());

        Eagle::RenderingContext::bind_shader(m_shader.lock());
        Eagle::RenderingContext::bind_descriptor_set(m_descriptorSet.lock());
        Eagle::RenderingContext::draw_indexed(m_vertexBuffer.lock(), m_indexBuffer.lock());

        Eagle::RenderingContext::end_draw_offscreen();

        Eagle::RenderingContext::bind_shader(postFx.shader.lock());
        Eagle::RenderingContext::bind_descriptor_set(postFx.descriptorSet.lock());
        Eagle::RenderingContext::draw_indexed(postFx.vertices.lock(), postFx.indices.lock());
    }

    virtual void handle_deattach() override{

    }

    virtual void handle_event(Eagle::Event& e) override {

        Eagle::EventDispatcher dispatcher(e);
        if (dispatcher.dispatch<Eagle::WindowResizedEvent>(BIND_EVENT_FN(SceneTest::handle_window_resized))){
            return;
        }
        if (dispatcher.dispatch<ButtonClickedEvent>(BIND_EVENT_FN(SceneTest::handle_button_pressed))){
            return;
        }
    }

    bool handle_button_pressed(ButtonClickedEvent &e){
        rotationDirection *= -1;
        color = glm::vec4(1.0f, 1.0, 1.0, 1.0f) - color;
        Eagle::RenderingContext::uniform_buffer_flush(m_colorBuffer.lock(), &color);
        return true;
    }


    bool handle_window_resized(Eagle::WindowResizedEvent& e){
        proj = glm::perspective(glm::radians(45.0f), (float)e.get_width() / e.get_height(), 0.1f, 10.0f);
        proj[1][1] *= -1;
        return true;
    }

};

#endif //EAGLE_SCENETEST_H
