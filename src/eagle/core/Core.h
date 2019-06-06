//
// Created by Novak on 05/05/2019.
//

#ifndef EAGLE_CORE_H
#define EAGLE_CORE_H

#include <map>

#define _EAGLE_BEGIN namespace Eagle {
#define _EAGLE_END   }

#define EAGLE_GET_INFO(key) Eagle::EagleInfo::get_info(key)
#define EAGLE_SET_INFO(key, value) Eagle::EagleInfo::set_info(key, value)

#define BIT(x) (1 << x)

_EAGLE_BEGIN
enum EAGLE_KEY{
    EAGLE_APP_NAME = 0,
    EAGLE_RENDERING_CONTEXT = 1,
    EAGLE_DEVICE_NAME = 2
};

class EagleInfo {
public:
    static std::string get_info(EAGLE_KEY key);
    static void set_info(EAGLE_KEY key, const std::string& value);
private:
    static std::map<EAGLE_KEY, std::string> m_infoMap;
};

_EAGLE_END


#endif //EAGLE_CORE_H
