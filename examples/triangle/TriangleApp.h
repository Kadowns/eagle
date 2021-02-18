//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/Eagle.h>

class TriangleApp : public Eagle::ApplicationDelegate {
public:
    TriangleApp();
    virtual ~TriangleApp() = default;

    void init() override;

    void step() override;

    void destroy() override;
private:
    Eagle::RenderingContext* m_renderingContext;
    Eagle::EventListener<TriangleApp> m_listener;
    Eagle::Handle<Eagle::Shader> m_shader;
    Eagle::Handle<Eagle::VertexBuffer> m_vertexBuffer;
    Eagle::Handle<Eagle::IndexBuffer> m_indexBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
