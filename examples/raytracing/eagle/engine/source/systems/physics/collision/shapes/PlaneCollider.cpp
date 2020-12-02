//
// Created by Novak on 5/4/2020.
//

#include <eagle/engine/components/physics/shapes/PlaneCollider.h>

EG_ENGINE_BEGIN

PlaneCollider::PlaneCollider(const glm::vec3 &normal, float distance) : m_normal(normal), m_distance(distance) {

}

const std::type_index& PlaneCollider::type() const {
    static std::type_index index(typeid(PlaneCollider));
    return index;
}

glm::vec3 PlaneCollider::compute_axis_inertia(Rigidbody &rigidbody) const {
    return glm::vec3(0, 0, 0);
}

void PlaneCollider::compute_aabb(const Rigidbody::Transform &transform) {
    m_aabb.max = glm::vec3(std::numeric_limits<float>::max());
    m_aabb.min = glm::vec3(std::numeric_limits<float>::min());
}

EG_ENGINE_END


