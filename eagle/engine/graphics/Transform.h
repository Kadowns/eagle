//
// Created by Novak on 29/07/2019.
//

#ifndef EAGLE_TRANSFORM_H
#define EAGLE_TRANSFORM_H

#include "eagle/engine/EngineCore.h"

EG_ENGINE_BEGIN

struct Transform {

    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) :
        position(position), scale(scale), m_rotation(rotation), m_eulerAngles(glm::eulerAngles(rotation)) {}

    glm::vec3 position;
    glm::vec3 scale;

    inline glm::vec3 get_euler() { return m_eulerAngles; }
    inline glm::quat get_rotation() { return m_rotation; }
    inline void set_euler(const glm::vec3& euler) {m_rotation = glm::quat(m_eulerAngles = euler); }
    inline void set_rotation(const glm::quat& rot) {m_eulerAngles = glm::eulerAngles(m_rotation = rot); }

    inline glm::vec3 front()    { return m_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    inline glm::vec3 up()       { return m_rotation * glm::vec3(0.0f, 1.0f, 0.0f);  }
    inline glm::vec3 right()    { return m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);  }
    inline glm::mat4& matrix()  { return m_matrix = glm::scale(glm::translate(glm::mat4(1), position) * glm::mat4_cast(m_rotation), scale);}

private:

    glm::quat m_rotation;
    glm::vec3 m_eulerAngles;
    glm::mat4 m_matrix;
};


EG_ENGINE_END

#endif //EAGLE_TRANSFORM_H
