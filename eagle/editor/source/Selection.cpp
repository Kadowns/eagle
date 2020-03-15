//
// Created by Novak on 06/08/2019.
//

#include <eagle/editor/Selection.h>

EG_EDITOR_BEGIN

Object::ObjectId Selection::s_currentObject;

TriggerEvent<Object*> Selection::on_object_selected;

void Selection::set_selected(Object* object) {
    s_currentObject = object->uid();
    on_object_selected(object);
}


bool Selection::is_selected(Object *object) {
    return s_currentObject == object->uid();
}

EG_EDITOR_END
