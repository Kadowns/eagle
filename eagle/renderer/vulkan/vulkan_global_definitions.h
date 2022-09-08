//
// Created by Novak on 02/06/2019.
//

#ifndef EAGLE_VULKANCORE_H
#define EAGLE_VULKANCORE_H

#include "eagle/core_global_definitions.h"
#include "eagle/renderer/renderer_global_definitions.h"
#include <vulkan/vulkan.h>

#ifdef VK_USE_PLATFORM_ANDROID_KHR
#include <eagle/renderer/vulkan/platform/android/vulkan_wrapper.h>
#endif

namespace eagle {

class VkDebugInfo {

public:

    struct VkCall {
        std::atomic<const char*> fileName;
        std::atomic<const char*> funcName;
        std::atomic<int> line;
    };

    inline static void set_call(const char* fileName, const char* funcName, int line) {
        m_callInfo.fileName= fileName;
        m_callInfo.funcName = funcName;
        m_callInfo.line = line;
    }

    inline static VkCall& get_last_call(){
        return m_callInfo;
    }

    static VkCall m_callInfo;

private:

};


}

#endif //EAGLE_VULKANCORE_H
