//
// Created by Novak on 29/07/2019.
//

#ifndef EAGLE_TRANSFORM_H
#define EAGLE_TRANSFORM_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/EntityManager.h>

EG_ENGINE_BEGIN

struct Transform : Component<Transform> {

    Transform() :
        position(glm::vec3(0)),scale(glm::vec3(1)), rotation(glm::vec3(0)){}

    Transform(glm::vec3 position, glm::quat rotation, glm::vec3 scale) :
        position(position), scale(scale), rotation(rotation) {}

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    inline glm::vec3 front()    { return rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    inline glm::vec3 up()       { return rotation * glm::vec3(0.0f, 1.0f, 0.0f);  }
    inline glm::vec3 right()    { return rotation * glm::vec3(1.0f, 0.0f, 0.0f);  }
    inline glm::mat4& matrix()  { return m_matrix = glm::scale(glm::translate(glm::mat4(1), position) * glm::mat4_cast(rotation), scale);}

private:
    glm::mat4 m_matrix;
};

EG_ENGINE_END

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, glm::vec3 & vec, const unsigned int version) {
    ar & vec.x;
    ar & vec.y;
    ar & vec.z;
}

template<class Archive>
void serialize(Archive & ar, glm::quat & quat, const unsigned int version) {
    ar & quat.x;
    ar & quat.y;
    ar & quat.z;
    ar & quat.w;
}

template<class Archive>
void serialize(Archive & ar, Eagle::Engine::Transform & transform, const unsigned int version) {
    ar & transform.position;
    ar & transform.rotation;
    ar & transform.scale;
}


} // namespace serialization
} // namespace boost



#endif //EAGLE_TRANSFORM_H
