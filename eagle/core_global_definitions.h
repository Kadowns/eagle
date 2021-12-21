//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_COREGLOBALDEFINITIONS_H
#define EAGLE_COREGLOBALDEFINITIONS_H

#include <queue>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <memory>

#include <cassert>
#include <mutex>
#include <random>
#include <functional>
#include <utility>
#include <typeindex>
#include <any>

#define BIT(x) (1 << x)

namespace eagle {

typedef uint32_t Flags;

enum class Cursor {
    ARROW = 0,
    TEXT = 1,
    VERT_RESIZE = 2,
    HORI_RESIZE = 3,
    HAND = 4,
    CROSSHAIR = 5
};

enum DataType {
    UNDEFINED,
    BOOL,
    FLOAT,
    INT,
    VECTOR2F,
    VECTOR3F,
    VECTOR4F,
    VECTOR2I,
    VECTOR3I,
    VECTOR4I,
    MATRIX2X2,
    MATRIX3X3,
    MATRIX4X4,
    EXTERNAL_IMAGE,
    EXTERNAL_SAMPLER,
    EXTERNAL_SAMPLED_IMAGE,
    EXTERNAL_BLOCK,
    EXTERNAL_MASK,
    STRUCT,
    ARRAY
};

}

#endif //EAGLE_COREGLOBALDEFINITIONS_H
