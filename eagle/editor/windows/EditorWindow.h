//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_EDITORWINDOW_H
#define EAGLE_EDITORWINDOW_H

#include <eagle/editor/EditorCore.h>

EG_EDITOR_BEGIN

class EditorWindow {
public:

    explicit EditorWindow(const std::string& name);
    virtual ~EditorWindow();

    virtual void update();
    virtual void draw(Scope <CommandBuffer> &commandBuffer) {}

protected:
    virtual void handle_update() = 0;
private:
    std::string m_name;
};

EG_EDITOR_END

#endif //EAGLE_EDITORWINDOW_H
