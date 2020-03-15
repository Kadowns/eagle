//
// Created by Novak on 01/09/2019.
//

#ifndef AUTOREGISTER_AUTOMATIC_REGISTER_H
#define AUTOREGISTER_AUTOMATIC_REGISTER_H

#include <eagle/editor/EditorCore.h>

EG_EDITOR_BEGIN

struct BaseEditor{
    virtual void handle_update() = 0;
    virtual ~BaseEditor(){}
};

template<typename D>
BaseEditor* create_object_editor(Object *obj) { return new D(obj); }

struct EditorFactory{

    static BaseEditor* create_for_object(Object *object){
        auto it = map()->find(object->type_name());
        if (it == map()->end()){
            EG_FATAL_F("No editor for object of type {0}", object->type_name());
            return nullptr;
        }
        return it->second(object);
    }


    static std::map<std::string, std::function<BaseEditor*(Object*)>>* map(){
        static auto* factoryMap = new std::map<std::string, std::function<BaseEditor*(Object*)>>();
        return factoryMap;
    }
};

template<typename E, typename R>
struct ObjectEditor :  BaseEditor {
public:
    virtual ~ObjectEditor(){}
private:

    struct Registrator {
        Registrator() {
            EditorFactory::map()->insert(std::make_pair(boost::typeindex::type_id<R>().pretty_name(),
                                                        &create_object_editor<E>));
        }
    };
    // will force instantiation of definition of static member
    template<Registrator&>
    struct RegistratorReferencer { };

    static Registrator s_registratorInstance;
    static RegistratorReferencer<s_registratorInstance> s_referencer;

};

template<typename E, typename R>
typename ObjectEditor<E, R>::Registrator ObjectEditor<E, R>::s_registratorInstance;

EG_EDITOR_END

#endif //AUTOREGISTER_AUTOMATIC_REGISTER_H
