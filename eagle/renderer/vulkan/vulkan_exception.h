//
// Created by Ricardo on 01/09/2022.
//

#ifndef EAGLE_VULKAN_EXCEPTION_H
#define EAGLE_VULKAN_EXCEPTION_H

#include <eagle/renderer/vulkan/vulkan_global_definitions.h>

#include <exception>

namespace eagle {

class VulkanException : public std::exception {
public:
    VulkanException(std::string what, VkResult resultCode);

    const char* what() const noexcept override;

    virtual VkResult code() const noexcept;

private:
    std::string m_what;
    VkResult m_resultCode;
};

}



#endif //EAGLE_VULKAN_EXCEPTION_H
