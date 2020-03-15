//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_INSPECTORWINDOW_H
#define EAGLE_INSPECTORWINDOW_H

#include "EditorWindow.h"
#include "ObjectEditor.h"

EG_EDITOR_BEGIN

class InspectorWindow : public EditorWindow {
public:
    InspectorWindow();
    virtual ~InspectorWindow();

protected:
    virtual void handle_update() override;

private:

    Reference<BaseEditor> m_currentEditor;

    TriggerEvent<Object*>::Listener m_onObjectSelectedCallback;

};

EG_EDITOR_END

#endif //EAGLE_INSPECTORWINDOW_H
