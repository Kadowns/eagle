//
// Created by Novak on 12/05/2019.
//

#include "../Core.h"
_EAGLE_BEGIN

std::map<EAGLE_KEY, std::string> EagleInfo::m_infoMap;

std::string EagleInfo::get_info(EAGLE_KEY key) {
    return m_infoMap[key];
}

void EagleInfo::set_info(EAGLE_KEY key, const std::string &value) {
    m_infoMap[key] = value;
}
_EAGLE_END