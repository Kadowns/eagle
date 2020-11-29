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

    void handle_event(Eagle::Event &e) override;

    void handle_deattach() override;
private:

    bool on_window_close(const Eagle::WindowCloseEvent& e);
    bool on_window_resized(const Eagle::WindowResizedEvent& e);


private:
    Eagle::Reference<Eagle::RenderingContext> m_renderingContext;
    Eagle::LayerEventDispatcher m_dispatcher;

    Eagle::Handle<Eagle::Shader> m_shader;
    Eagle::Handle<Eagle::VertexBuffer> m_vertexBuffer;

};


#endif //TRIANGLELAYER_H
