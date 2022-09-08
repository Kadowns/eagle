//
// Created by Ricardo on 01/09/2022.
//

#include <eagle/renderer/vulkan/vulkan_exception.h>

namespace eagle {

VulkanException::VulkanException(std::string what, VkResult resultCode) :
    m_what(std::move(what)),
    m_resultCode(resultCode) {
}

const char* VulkanException::what() const noexcept {
    return m_what.c_str();
}

VkResult VulkanException::code() const noexcept {
    return m_resultCode;
}

}