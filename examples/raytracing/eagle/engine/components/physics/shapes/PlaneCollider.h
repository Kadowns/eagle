//
// Created by Novak on 5/4/2020.
//

#ifndef EAGLE_PLANECOLLIDER_H
#define EAGLE_PLANECOLLIDER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/shapes/ColliderShape.h>

EG_ENGINE_BEGIN

class PlaneCollider : public ColliderShape {
public:
    PlaneCollider(const glm::vec3& normal, float distance);
    virtual const std::type_index &type() const override;

    virtual void compute_aabb(const Rigidbody::Transform &transform) override;

    virtual glm::vec3 compute_axis_inertia(Rigidbody& rigidbody)const override;
    inline float distance() const { return m_distance; }
    inline glm::vec3 normal() const { return m_normal; }
private:
    glm::vec3 m_normal;
    float m_distance;
};


EG_ENGINE_END

#endif //EAGLE_PLANECOLLIDER_H
