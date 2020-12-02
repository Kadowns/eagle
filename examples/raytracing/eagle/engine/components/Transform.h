//
// Created by Novak on 30/03/2020.
//

#ifndef EAGLE_TRANSFORM_H
#define EAGLE_TRANSFORM_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Transform {
    Transform() : m_position(0), m_rotation(glm::vec3(0)), m_scale(1) {}
    Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) :
    m_position(position), m_rotation(rotation), m_scale(scale) {}

    inline void set_position(const glm::vec3& position) {
        if (m_position != position){
            hasChanged = true;
        }
        m_position = position;
    }

    inline void set_rotation(const glm::quat& rotation) {
        double const eps = 1e-29; // some error threshold
        if (glm::abs(glm::dot(m_rotation, rotation)) < 1 - eps){
            hasChanged = true;
        }
        m_rotation = rotation;

    }

    inline void set_scale(const glm::vec3& scale){
        if (m_scale != scale){
            hasChanged = true;
        }
        m_scale = scale;
    }

    inline void translate(const glm::vec3& amount){
        set_position(m_position + amount);
    }

    inline glm::vec3 position() const { return m_position; }
    inline glm::quat rotation() const { return m_rotation; }
    inline glm::vec3 front()    const { return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    inline glm::vec3 up()       const { return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);  }
    inline glm::vec3 right()    const { return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);  }
    inline glm::mat4 matrix()   const { return glm::scale(glm::translate(glm::mat4(1), m_position) * glm::mat4_cast(m_rotation), m_scale);}

    bool hasChanged;

private:

    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
};


EG_ENGINE_END

#endif //EAGLE_TRANSFORM_H
