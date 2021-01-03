//
// Created by Ricardo on 11/28/2020.
//

#ifndef TRIANGLELAYER_H
#define TRIANGLELAYER_H

#include <eagle/Eagle.h>

class TriangleLayer : public Eagle::Layer {
public:
    void handle_attach() override;

    void handle_update() override;

    void handle_detach() override;

    bool receive(const Eagle::OnWindowClose& e);

private:
    Eagle::Reference<Eagle::RenderingContext> m_renderingContext;
    Eagle::EventListener<TriangleLayer> m_listener;

    Eagle::Handle<Eagle::Shader> m_shader;
    Eagle::Handle<Eagle::VertexBuffer> m_vertexBuffer;
    Eagle::Handle<Eagle::IndexBuffer> m_indexBuffer;

};


#endif //TRIANGLELAYER_H
