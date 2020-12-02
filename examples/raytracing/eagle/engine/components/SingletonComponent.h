//
// Created by Novak on 11/04/2020.
//

#ifndef EAGLE_SINGLETONCOMPONENT_H
#define EAGLE_SINGLETONCOMPONENT_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

class SingletonComponent {
public:
    template<typename T>
    static T& get(){
        static SingletonComponent instance;
        return instance.handle_get<T>();
    }
private:

    template<typename T>
    T& handle_get(){
        static std::type_index index(typeid(T));
        if (m_components.find(index) == m_components.end()){
            m_components.emplace(index, T());
        }
        return std::any_cast<T&>(m_components[index]);
    }

private:

    std::unordered_map<std::type_index, std::any> m_components;
};


EG_ENGINE_END

#endif //EAGLE_SINGLETONCOMPONENT_H
