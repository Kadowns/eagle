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
    nativeImageCreateInfo.commandPool = nativeCreateInfo.commandPool;
    nativeImageCreateInfo.graphicsQueue = nativeCreateInfo.graphicsQueue;
    nativeImageCreateInfo.imageCount = nativeCreateInfo.imageCount;

    m_image = std::make_shared<VulkanImage>(textureCreateInfo.imageCreateInfo, nativeImageCreateInfo);
    create();
    EG_TRACE("eagle","Vulkan texture constructed!");
}

VulkanTexture::~VulkanTexture() {
    EG_TRACE("eagle","Destroying a vulkan texture!");
    clear();
    EG_TRACE("eagle","Vulkan texture destroyed!");
}

void VulkanTexture::create() {
    EG_TRACE("eagle","Creating a vulkan texture!");
    VK_CALL VulkanHelper::create_image_sampler(
            m_nativeCreateInfo.device,
            m_sampler,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VulkanConverter::to_vk(m_createInfo.filter)
    );
    EG_TRACE("eagle","Vulkan texture created!");
}

void VulkanTexture::clear() {
    EG_TRACE("eagle","Clearing a vulkan texture!");
    if (m_sampler){
        VK_CALL vkDestroySampler(m_nativeCreateInfo.device, m_sampler, nullptr);
    }
    m_image.reset();
    EG_TRACE("eagle","Vulkan texture cleared!");
}

void VulkanTexture::resize(uint32_t width, uint32_t height) {
    EG_TRACE("eagle","Resizing a vulkan texture!");
    clear();
    m_image->resize(width, height);
    create();
    EG_TRACE("eagle","Vulkan texture resized!");
}

DescriptorType VulkanTexture::type() const {
    return DescriptorType::COMBINED_IMAGE_SAMPLER;
}

}
