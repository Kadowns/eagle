//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_COREGLOBALDEFINITIONS_H
#define EAGLE_COREGLOBALDEFINITIONS_H

#include <queue>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <memory>

#include <random>
#include <functional>
#include <utility>
#include <typeindex>
#include <any>

#define EG_BEGIN namespace Eagle {
#define EG_END   }

#define EAGLE_GET_INFO(key) Eagle::EagleInfo::get_info(key)
#define EAGLE_SET_INFO(key, value) Eagle::EagleInfo::set_info(key, value)

#define BIT(x) (1 << x)

EG_BEGIN

template<typename T>
using Handle = std::weak_ptr<T>;

template<typename T>
using Reference = std::shared_ptr<T>;

template<typename T>
using Scope = std::unique_ptr<T>;

enum EAGLE_KEY{
    EAGLE_APP_NAME = 0,
    EAGLE_RENDERING_CONTEXT = 1,
    EAGLE_DEVICE_NAME = 2
};

enum class Cursor{
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

class EagleInfo {
public:
    static std::string get_info(EAGLE_KEY key);
    static void set_info(EAGLE_KEY key, const std::string& value);
private:
    static std::map<EAGLE_KEY, std::string> m_infoMap;
};

EG_END

#endif //EAGLE_COREGLOBALDEFINITIONS_H
