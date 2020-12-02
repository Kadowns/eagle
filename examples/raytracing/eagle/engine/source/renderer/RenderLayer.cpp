//
// Created by Novak on 17/03/2020.
//

#include <eagle/engine/renderer/RenderLayer.h>

EG_ENGINE_BEGIN


void RenderLayer::handle_attach() {
    m_dispatcher.add_listener<WindowResizedEvent>([&](Event& e){
        auto& resized = *(WindowResizedEvent*)&e;
        m_renderMaster.handle_window_resized(resized.get_width(), resized.get_height());
        return false;
    });

    m_dispatcher.add_listener<WindowCloseEvent>([&](Event& e){
        Application::instance().quit();
        return false;
    });

    m_renderMaster.init();
}

void RenderLayer::handle_deattach() {
    m_renderMaster.deinit();
}

void RenderLayer::handle_update() {
    m_renderMaster.update();
}

void RenderLayer::handle_event(Event &e) {
    m_dispatcher.dispatch(e);

}

EG_ENGINE_END
