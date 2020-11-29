//
// Created by Novak on 18/07/2019.
//

#ifndef EAGLE_INPUT_H
#define EAGLE_INPUT_H

#include <memory>
#include <set>

#include "CoreGlobalDefinitions.h"
#include "events/InputEvents.h"
#include "events/KeyCodes.h"

EG_BEGIN

class Input {
private:
    using Position = std::pair<double, double>;

public:
    ~Input() = default;

    static Input& instance();

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

    friend class InputLayer;
    void handle_key(KeyEvent& e);
    void handle_mouse_move(MouseMoveEvent& e);
    void handle_mouse_button(MouseButtonEvent& e);
    void handle_mouse_scroll(MouseScrolledEvent& e);

private:

    static Scope<Input> s_instance;

    std::set<int> m_downKeys, m_pressedKeys, m_releasedKeys;
    std::set<int> m_downMouseButtons, m_pressedMouseButtons, m_releasedMouseButtons;

    bool m_firstMouseMove = true;
    Position m_mousePosition, m_mouseDelta, m_scrollDelta;

};


EG_END

#endif //EAGLE_INPUT_H
