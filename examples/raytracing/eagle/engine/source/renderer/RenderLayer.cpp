//
// Created by Novak on 17/03/2020.
//

#include <eagle/engine/renderer/RenderLayer.h>

EG_ENGINE_BEGIN

void RenderLayer::handle_attach(EventBus<EventStream>* eventBus) {
    m_listener.attach(eventBus);
    m_listener.subscribe<OnWindowResized>([this](const OnWindowResized& ev){
       m_renderMaster.handle_window_resized(ev.width, ev.height);
       return false;
    });

    m_listener.subscribe<OnWindowClose>([this](const OnWindowClose& ev){
        Application::instance().quit();
        return false;
    });

    m_renderMaster.init();
}

void RenderLayer::handle_detach() {
    m_renderMaster.deinit();
    m_listener.detach();
}

void RenderLayer::handle_update() {
    m_renderMaster.update();
}

EG_ENGINE_END
