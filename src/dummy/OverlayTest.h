//
// Created by Novak on 12/06/2019.
//

#ifndef EAGLE_OVERLAYTEST_H
#define EAGLE_OVERLAYTEST_H

#include <eagle/Eagle.h>

class OverlayTest : public Eagle::Layer {
private:

    std::weak_ptr<Eagle::Shader> m_shader;
    std::weak_ptr<Eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<Eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<Eagle::UniformBuffer> m_uniformBuffer;
    std::weak_ptr<Eagle::DescriptorSet> m_descriptorSet;


    glm::mat4 proj, model;

public:
    virtual void handle_attach() override {
        m_shader = Eagle::RenderingContext::create_shader("shaders/shader_vert.spv", "shaders/shader_frag.spv");

        std::vector<float> vertices = {
                0.0f, 0.0f, 0.0f, 1.0f, 0.8f, 0.8, // 0
                0.0f, 1.0f, 0.0f, 1.0f, 0.8f, 0.8,  // 1
                1.0f, 1.0f, 0.0f, 1.0f, 0.8f, 0.8,  // 2
                1.0f, 0.0f, 0.0f, 1.0f, 0.8f, 0.8,  // 3
        };
        m_vertexBuffer = Eagle::RenderingContext::create_vertex_buffer(vertices, 6);

        std::vector<uint32_t> indices = {
                0, 1, 2, 0, 2, 3,
        };
        m_indexBuffer = Eagle::RenderingContext::create_index_buffer(indices);

        m_uniformBuffer = Eagle::RenderingContext::create_uniform_buffer(m_shader.lock()->get_shader_item("mvp"));



        m_descriptorSet = Eagle::RenderingContext::create_descriptor_set(m_shader.lock(), {m_uniformBuffer.lock()});


        //view = glm::lookAt(glm::vec3(0.0f, 3.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        proj = glm::ortho(0.0f, (float)Eagle::Application::get_window_width(), 0.0f, (float)Eagle::Application::get_window_height());
        model = glm::translate(glm::mat4(1), glm::vec3(0, (float)Eagle::Application::get_window_height() - 128, 0));
        model = glm::scale(model, glm::vec3((float)Eagle::Application::get_window_width(), 128, 1));

    }

    virtual void handle_deattach() override {

    }

    virtual void handle_update() override {

    }

    virtual void handle_draw() override {
        glm::mat4 pm = proj * model;
        Eagle::RenderingContext::flush_uniform_buffer_data(m_uniformBuffer.lock(), &pm);
        Eagle::RenderingContext::bind_shader(m_shader.lock());
        Eagle::RenderingContext::bind_descriptor_set(m_descriptorSet.lock());
        Eagle::RenderingContext::draw_indexed_vertex_buffer(m_vertexBuffer.lock(), m_indexBuffer.lock());
    }

    virtual void handle_event(Eagle::Event &e) override {

    }

};


#endif //EAGLE_OVERLAYTEST_H
