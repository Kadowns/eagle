//
// Created by Novak on 5/3/2020.
//

#ifndef EAGLE_BOXCOLLIDER_H
#define EAGLE_BOXCOLLIDER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/shapes/ColliderShape.h>

EG_ENGINE_BEGIN

class BoxCollider : public ColliderShape {
public:
    BoxCollider(const glm::vec3 extents);
    virtual const std::type_index &type() const override;
    virtual glm::vec3 compute_axis_inertia(Rigidbody& rigidbody) const override;
    inline glm::vec3 extents() const { return m_extents; }
    inline std::array<glm::vec3, 8> vertices() const { return m_vertices; }

    virtual void compute_aabb(const Rigidbody::Transform &transform) override;

private:
    glm::vec3 m_extents;
    std::array<glm::vec3, 8> m_vertices;
};

EG_ENGINE_END

#endif //EAGLE_BOXCOLLIDER_H
