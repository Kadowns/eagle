//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_EDITORWINDOW_H
#define EAGLE_EDITORWINDOW_H

#include "eagle/engine/EngineCore.h"

EG_ENGINE_BEGIN

class EditorWindow {
public:

    explicit EditorWindow(const std::string& name);
    virtual ~EditorWindow();

    virtual void update();
    virtual void draw() {}

protected:
    virtual void handle_update() = 0;
private:
    std::string m_name;
};

EG_ENGINE_END

#endif //EAGLE_EDITORWINDOW_H
