//
// Created by Novak on 01/09/2019.
//

#ifndef EAGLE_OBJECT_H
#define EAGLE_OBJECT_H

#include "GlobalDefs.h"

EG_ENGINE_BEGIN

class Object {
public:
    using ObjectId = size_t;
public:

    explicit Object(const std::string& name) : m_name(name) {
        m_id = ++s_globalIdCounter;
    }

    Object() : m_name("object"){
        m_id = ++s_globalIdCounter;
    }

    //useful for object factories
    virtual std::string type_name() const = 0;

    inline const std::string& name() const { return m_name; }

    inline bool operator==(const Object& other) const {
        return m_id == other.m_id;
    }
    inline bool operator!=(const Object& other) const {
        return m_id != other.m_id;
    }

    inline ObjectId uid() const { return m_id; }


protected:

    //global id counter
    static ObjectId  s_globalIdCounter;

    //unique id for this object (not persistent)
    ObjectId  m_id;

    std::string m_name;
};

EG_ENGINE_END

#endif //EAGLE_OBJECT_H
