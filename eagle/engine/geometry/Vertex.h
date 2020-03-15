//
// Created by Novak on 24/11/2019.
//

#ifndef EAGLE_VERTEX_H
#define EAGLE_VERTEX_H

#include "eagle/engine/GlobalDefs.h"

EG_ENGINE_BEGIN

struct Vertex {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};

EG_ENGINE_END

#endif //EAGLE_VERTEX_H
