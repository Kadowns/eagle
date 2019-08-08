//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_INSPECTORWINDOW_H
#define EAGLE_INSPECTORWINDOW_H

#include "EditorWindow.h"

EG_ENGINE_BEGIN

class InspectorWindow : public EditorWindow {
public:
    InspectorWindow();
    virtual ~InspectorWindow();

protected:
    virtual void handle_update() override;

};

EG_ENGINE_END

#endif //EAGLE_INSPECTORWINDOW_H
