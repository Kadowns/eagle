//
// Created by Novak on 5/3/2020.
//

#include <eagle/engine/components/physics/shapes/SphereCollider.h>

EG_ENGINE_BEGIN

SphereCollider::SphereCollider(float radius) : m_radius(radius){

}

const std::type_index &SphereCollider::type() const {
    static std::type_index index(typeid(SphereCollider));
    return index;
}

glm::vec3 SphereCollider::compute_axis_inertia(Rigidbody& rigidbody) const {
    // Calculate inertia tensor
    float r2 =  4.0 * m_radius * m_radius;
    float i =  1.0 / 12.0 * rigidbody.mass * (r2 + r2);
    return glm::vec3(i);
}

void SphereCollider::compute_aabb(const Rigidbody::Transform &transform) {
    glm::vec3 radius = glm::vec3(m_radius);
    m_aabb.min = transform.position - radius;
    m_aabb.max = transform.position + radius;
}

EG_ENGINE_END
