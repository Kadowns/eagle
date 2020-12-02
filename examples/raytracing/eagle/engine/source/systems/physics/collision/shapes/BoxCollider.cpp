//
// Created by Novak on 5/3/2020.
//

#include <eagle/engine/components/physics/shapes/BoxCollider.h>

EG_ENGINE_BEGIN
BoxCollider::BoxCollider(const glm::vec3 extents) : m_extents(extents){
    m_vertices = {
            glm::vec3( -m_extents.x, -m_extents.y, -m_extents.z ),
            glm::vec3( -m_extents.x, -m_extents.y,  m_extents.z ),
            glm::vec3( -m_extents.x,  m_extents.y, -m_extents.z ),
            glm::vec3( -m_extents.x,  m_extents.y,  m_extents.z ),
            glm::vec3(  m_extents.x, -m_extents.y, -m_extents.z ),
            glm::vec3(  m_extents.x, -m_extents.y,  m_extents.z ),
            glm::vec3(  m_extents.x,  m_extents.y, -m_extents.z ),
            glm::vec3(  m_extents.x,  m_extents.y,  m_extents.z )
    };
}

const std::type_index &BoxCollider::type() const {
    static std::type_index index(typeid(BoxCollider));
    return index;
}

glm::vec3 BoxCollider::compute_axis_inertia(Rigidbody &rigidbody) const {
    float ex2 =  4.0 * m_extents.x * m_extents.x;
    float ey2 =  4.0 * m_extents.y * m_extents.y;
    float ez2 =  4.0 * m_extents.z * m_extents.z;
    float x =  1.0 / 12.0 * rigidbody.mass * (ex2 + ex2);
    float y =  1.0 / 12.0 * rigidbody.mass * (ey2 + ey2);
    float z =  1.0 / 12.0 * rigidbody.mass * (ez2 + ez2);
    return glm::vec3(x, y, z);
}

void BoxCollider::compute_aabb(const Rigidbody::Transform &transform) {
    m_aabb.max = glm::vec3(-std::numeric_limits<float>::max());
    m_aabb.min = glm::vec3(std::numeric_limits<float>::max());
    for (uint32_t i = 0; i < m_vertices.size(); i++){
        glm::vec3 v = (transform.rotation * m_vertices[i]) + transform.position;
        m_aabb.max = glm::max(m_aabb.max, v);
        m_aabb.min = glm::min(m_aabb.min, v);
    }
}


EG_ENGINE_END
