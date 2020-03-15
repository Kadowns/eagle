//
// Created by Novak on 02/09/2019.
//

#ifndef EAGLE_ENTITYEDITOR_H
#define EAGLE_ENTITYEDITOR_H

#include "eagle/editor/windows/ObjectEditor.h"

EG_EDITOR_BEGIN

struct EntityEditor : ObjectEditor<EntityEditor, Entity> {
    EntityEditor(Object* object);
    virtual void handle_update() override;

    Entity* entity;
};

EG_EDITOR_END

#endif //EAGLE_ENTITYEDITOR_H
