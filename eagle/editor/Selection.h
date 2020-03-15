//
// Created by Novak on 06/08/2019.
//

#ifndef EAGLE_SELECTION_H
#define EAGLE_SELECTION_H

#include "EditorCore.h"

EG_EDITOR_BEGIN

class Selection {
public:

    static void set_selected(Object* object);

    static bool is_selected(Object* object);

    static TriggerEvent<Object*> on_object_selected;

private:
    static Object::ObjectId s_currentObject;
};

EG_EDITOR_END

#endif //EAGLE_SELECTION_H
