//
// Created by Novak on 17/03/2020.
//

#include <eagle/engine/renderer/RenderLayer.h>

EG_ENGINE_BEGIN

void RenderLayer::handle_attach() {
    m_listener.attach(&DesktopApplication::instance().event_bus());

    m_listener.subscribe<OnWindowClose>([](const OnWindowClose& ev){
        DesktopApplication::instance().quit();
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
