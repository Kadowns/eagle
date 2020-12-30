//
// Created by Novak on 10/11/2019.
//

#include <eagle/core/InputLayer.h>
#include <eagle/core/events/WindowEvents.h>
#include <eagle/core/events/InputEvents.h>
#include <eagle/core/Input.h>

EG_BEGIN

void InputLayer::handle_attach(EventBus<EventStream> *eventBus) {
    Input::instance().init(eventBus);
}

void InputLayer::handle_detach() {
    Input::instance().deinit();
}

void InputLayer::handle_update() {
    Input::instance().refresh();
}

EG_END
