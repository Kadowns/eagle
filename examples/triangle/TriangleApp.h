//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/Eagle.h>

class TriangleApp : public eagle::ApplicationDelegate {
public:
    TriangleApp();
    virtual ~TriangleApp() = default;

    void init() override;

    void step() override;

    void destroy() override;
private:
    eagle::RenderingContext* m_renderingContext;
    eagle::EventListener<TriangleApp> m_listener;
    std::weak_ptr<eagle::Shader> m_shader;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_primaryCommandBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_secondaryCommandBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
