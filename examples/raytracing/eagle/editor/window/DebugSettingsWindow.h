//
// Created by Novak on 27/05/2020.
//

#ifndef EAGLE_DEBUGSETTINGSWINDOW_H
#define EAGLE_DEBUGSETTINGSWINDOW_H

#include <eagle/editor/window/EditorWindow.h>

EG_EDITOR_BEGIN

class DebugSettingsWindow : public EditorWindow {
public:
    DebugSettingsWindow(EventBus* eventBus);
    virtual ~DebugSettingsWindow();

    virtual void handle_window_update() override;
private:
    EventBus* m_eventBus;

};

EG_EDITOR_END


#endif //EAGLE_DEBUGSETTINGSWINDOW_H
