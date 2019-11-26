//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_CORE_H
#define EAGLE_CORE_H

#include <memory>
#include <map>

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

enum class EG_CURSOR{
    ARROW = 0,
    TEXT = 1,
    VERT_RESIZE = 2,
    HORI_RESIZE = 3,
    HAND = 4,
    CROSSHAIR = 5
};

class EagleInfo {
public:
    static std::string get_info(EAGLE_KEY key);
    static void set_info(EAGLE_KEY key, const std::string& value);
private:
    static std::map<EAGLE_KEY, std::string> m_infoMap;
};

EG_END


#endif //EAGLE_CORE_H
