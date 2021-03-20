//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>

class TriangleApplication : public eagle::ApplicationDelegate {
public:
    TriangleApplication();
    ~TriangleApplication() override = default;

    void init() override;

    void step() override;

    void destroy() override;

private:

    void bake_command_buffers();

private:
    eagle::RenderingContext* m_renderingContext;
    eagle::EventListener<TriangleApplication*> m_listener;
    std::weak_ptr<eagle::Shader> m_shader;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_primaryCommandBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_secondaryCommandBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
