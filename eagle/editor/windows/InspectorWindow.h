//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_INSPECTORWINDOW_H
#define EAGLE_INSPECTORWINDOW_H

#include "EditorWindow.h"

EG_EDITOR_BEGIN

class InspectorWindow : public EditorWindow {
public:
    InspectorWindow();
    virtual ~InspectorWindow();

protected:
    virtual void handle_update() override;

    void draw_entity_inspector(entityx::Entity e);
    void draw_resource_inspector(BaseResource* resource);

};

EG_EDITOR_END

#endif //EAGLE_INSPECTORWINDOW_H
