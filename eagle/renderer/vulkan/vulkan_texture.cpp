//
// Created by Novak on 19/06/2019.
//

#include <eagle/renderer/vulkan/vulkan_converter.h>
#include "vulkan_texture.h"
#include "vulkan_helper.h"

namespace eagle {

VulkanTexture::VulkanTexture(const TextureCreateInfo &textureCreateInfo,
                                 const VulkanTextureCreateInfo &nativeCreateInfo) :
    Texture(textureCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo) {
    EG_TRACE("eagle","Constructing a vulkan texture!");

    VulkanImageCreateInfo nativeImageCreateInfo = {};
    nativeImageCreateInfo.device = nativeCreateInfo.device;
    nativeImageCreateInfo.physicalDevice = nativeCreateInfo.physicalDevice;
    nativeImageCreateInfo.frameCount = nativeCreateInfo.frameCount;

    m_image = std::make_shared<VulkanImage>(textureCreateInfo.imageCreateInfo, nativeImageCreateInfo);
    create();
}

VulkanTexture::~VulkanTexture() {
    clear();
}

void VulkanTexture::create() {

    VulkanHelper::create_image_sampler(
            m_nativeCreateInfo.device,
            m_sampler,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VulkanConverter::to_vk(m_createInfo.filter),
            m_createInfo.imageCreateInfo.mipLevels);
}

void VulkanTexture::clear() {
    if (m_sampler){
        vkDestroySampler(m_nativeCreateInfo.device, m_sampler, nullptr);
    }
    m_image.reset();
}

void VulkanTexture::resize(uint32_t width, uint32_t height) {
    clear();
    m_image->resize(width, height);
    create();
}

DescriptorType VulkanTexture::type() const {
    return DescriptorType::COMBINED_IMAGE_SAMPLER;
}

}
