//
// Created by Novak on 5/3/2020.
//

#ifndef EAGLE_SPHERECOLLIDER_H
#define EAGLE_SPHERECOLLIDER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/shapes/ColliderShape.h>

EG_ENGINE_BEGIN

class SphereCollider : public ColliderShape {
public:
    SphereCollider(float radius);
    virtual const std::type_index &type() const override;
    virtual glm::vec3 compute_axis_inertia(Rigidbody& rigidbody)const override;

    virtual void compute_aabb(const Rigidbody::Transform &transform) override;

    inline float radius() const { return m_radius; }
private:
    float m_radius;
};


EG_ENGINE_END

#endif //EAGLE_SPHERECOLLIDER_H
