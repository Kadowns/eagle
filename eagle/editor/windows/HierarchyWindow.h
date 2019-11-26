//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_HIERARCHYWINDOW_H
#define EAGLE_HIERARCHYWINDOW_H

#include "EditorWindow.h"

EG_EDITOR_BEGIN

class HierarchyWindow : public EditorWindow {
public:
    HierarchyWindow();
    virtual ~HierarchyWindow();

protected:
    virtual void handle_update() override;
private:
    Reference<entityx::EntityManager> m_entities;
};

EG_EDITOR_END


#endif //EAGLE_HIERARCHYWINDOW_H
