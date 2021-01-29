//
// Created by Novak on 10/11/2019.
//

#include <eagle/core/InputLayer.h>
#include <eagle/core/Input.h>
#include <eagle/core/Application.h>

EG_BEGIN

void InputLayer::handle_attach() {
    Input::instance().init(&Application::instance().event_bus());
}

void InputLayer::handle_detach() {
    Input::instance().deinit();
}

void InputLayer::handle_update() {
    Input::instance().refresh();
}

EG_END
