//
// Created by Novak on 01/09/2019.
//

#ifndef AUTOREGISTER_FOOS_H
#define AUTOREGISTER_FOOS_H

#include "eagle/editor/windows/ObjectEditor.h"

EG_EDITOR_BEGIN

struct MaterialEditor : ObjectEditor<MaterialEditor, Material> {
    MaterialEditor(Object* object);
    virtual void handle_update() override;

    ResourceHandle<Material> material;
};

EG_EDITOR_END

#endif //AUTOREGISTER_FOOS_H
