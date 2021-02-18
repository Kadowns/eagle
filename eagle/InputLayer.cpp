//
// Created by Novak on 10/11/2019.
//

#include <eagle/InputLayer.h>
#include <eagle/Input.h>
#include <eagle/platform/desktop/DesktopApplication.h>

EG_BEGIN

void InputLayer::handle_attach() {
//    Input::instance().init(&DesktopApplication::instance().event_bus());
}

void InputLayer::handle_detach() {
    Input::instance().deinit();
}

void InputLayer::handle_update() {
    Input::instance().refresh();
}

EG_END
