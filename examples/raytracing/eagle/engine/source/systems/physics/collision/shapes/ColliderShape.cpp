//
// Created by Novak on 5/3/2020.
//

#include <eagle/engine/components/physics/shapes/ColliderShape.h>

EG_ENGINE_BEGIN

void ColliderShape::compute_inertia(Rigidbody &rigidbody) const {
    if (rigidbody.mass != 0.0f){
        rigidbody.inverseInertiaModel = glm::inverse(glm::diagonal3x3(compute_axis_inertia(rigidbody)));
    }
    else {
        rigidbody.inverseInertiaModel = glm::diagonal3x3(glm::vec3(0));
        rigidbody.inverseInertiaWorld = rigidbody.inverseInertiaModel;
    }

}


EG_ENGINE_END


