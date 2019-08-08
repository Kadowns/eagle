//
// Created by Novak on 02/06/2019.
//

#ifndef EAGLE_VULKANCORE_H
#define EAGLE_VULKANCORE_H

#include "eagle/core/Core.h"
#include "eagle/renderer/RenderingCore.h"
#include <vulkan/vulkan.h>

_EAGLE_BEGIN

class VkDebugInfo {

public:

    struct VkCall {
        std::string fileName;
        std::string funcName;
        int line;
    };

    inline static void set_call(const char* fileName, const char* funcName, int line) {
        m_callInfo.fileName= fileName;
        m_callInfo.funcName = funcName;
        m_callInfo.line = line;
    }

    inline static VkCall& get_last_call(){
        return m_callInfo;
    }

private:

    static VkCall m_callInfo;

};


_EAGLE_END

#define VK_CALL Eagle::VkDebugInfo::set_call(FILE_BASENAME, __FUNCTION__, __LINE__);

#define VK_CALL_ASSERT(result) VK_CALL if (result != VK_SUCCESS)


#endif //EAGLE_VULKANCORE_H
