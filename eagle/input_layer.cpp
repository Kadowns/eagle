//
// Created by Novak on 10/11/2019.
//

#include <eagle/input_layer.h>
#include <eagle/input.h>
#include <eagle/platform/desktop/desktop_application.h>

namespace eagle {

void InputLayer::handle_attach() {
//    Input::instance().init(&DesktopApplication::instance().event_bus());
}

void InputLayer::handle_detach() {
    Input::instance().deinit();
}

void InputLayer::handle_update() {
    Input::instance().refresh();
}

}
