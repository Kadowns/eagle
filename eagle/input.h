//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_INPUT_H
#define EAGLE_INPUT_H

#include "core_global_definitions.h"
#include "eagle/events/event.h"
#include "eagle/events/input_events.h"
#include "eagle/events/key_codes.h"

namespace eagle {

class Input {
private:
    using Position = std::pair<double, double>;

public:
    ~Input() = default;

    static Input& instance();

    void init(EventBus* eventBus);
    void deinit();

    void refresh();

    //keyboard
    bool key_pressed(int key);
    bool key_down(int key);
    bool key_released(int key);

    //mouse
    bool mouse_button_down(int button);
    bool mouse_button_pressed(int button);
    bool mouse_button_released(int button);
    Position mouse_position();
    Position mouse_move_delta();
    Position mouse_scroll_delta();
    float mouse_x();
    float mouse_y();

protected:

    friend class GenericEventListener<EventBus>;
    bool receive(const OnKey &e);
    bool receive(const OnMouseMove &e);
    bool receive(const OnMouseButton &e);
    bool receive(const OnMouseScrolled &e);

private:

    static std::unique_ptr<Input> s_instance;

    std::set<int> m_downKeys, m_pressedKeys, m_releasedKeys;
    std::set<int> m_downMouseButtons, m_pressedMouseButtons, m_releasedMouseButtons;

    Position m_mousePosition, m_mouseDelta, m_scrollDelta;
    EventListener m_listener;
    bool m_firstMouseMove = true;

};


}

#endif //EAGLE_INPUT_H
