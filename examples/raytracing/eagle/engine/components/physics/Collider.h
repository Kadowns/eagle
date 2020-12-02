//
// Created by Novak on 5/2/2020.
//

#ifndef EAGLE_COLLIDER_H
#define EAGLE_COLLIDER_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/components/physics/shapes/ColliderShape.h>

EG_ENGINE_BEGIN

struct Collider {
    Collider(Reference<ColliderShape>&& shape, uint32_t layer = 1) : shape(std::move(shape)), layer(layer){}
    Reference<ColliderShape> shape;
    uint32_t layer;
};

EG_ENGINE_END

#endif //EAGLE_COLLIDER_H
