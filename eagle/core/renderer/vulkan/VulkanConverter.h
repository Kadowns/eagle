//
// Created by Novak on 31/08/2019.
//

#ifndef EAGLE_VULKANCONVERTER_H
#define EAGLE_VULKANCONVERTER_H

#include <eagle/core/renderer/RenderPass.h>
#include "VulkanCore.h"

EG_BEGIN

class VulkanConverter {
public:
    static VkFormat to_vk(Format format);
    static Format to_eg(VkFormat format);
    static VkShaderStageFlags to_vk(ShaderStage stage);
    static ShaderStage to_eg(VkShaderStageFlags stage);
    static VkDescriptorType to_vk(DescriptorType type);
    static DescriptorType to_eg(VkDescriptorType type);
    static VkFilter to_vk(Filter filter);
    static Filter to_eg(VkFilter filter);
    static VkPrimitiveTopology to_vk(PrimitiveTopology topology);
    static PrimitiveTopology to_eg(VkPrimitiveTopology topology);
    static AttachmentLoadOperator to_eg(VkAttachmentLoadOp op);
    static VkAttachmentLoadOp to_vk(AttachmentLoadOperator op);
    static AttachmentStoreOperator to_eg(VkAttachmentStoreOp op);
    static VkAttachmentStoreOp to_vk(AttachmentStoreOperator op);
    static VkImageLayout to_vk(ImageLayout op);
    static ImageLayout to_eg(VkImageLayout op);
    static VkAttachmentDescription to_vk(const RenderAttachmentDescription &egAttachment);
    static RenderAttachmentDescription to_eg(const VkAttachmentDescription &vkAttachment);
    static VkImageTiling to_vk(ImageTiling tiling);
    static ImageTiling to_eg(VkImageTiling tiling);
    static ImageUsage to_eg(VkImageUsageFlagBits usage);
    static VkImageUsageFlagBits to_vk(ImageUsage usage);
    static ImageAspect to_eg(VkImageAspectFlagBits usage);
    static VkImageAspectFlagBits to_vk(ImageAspect usage);
    static MemoryProperty to_eg(VkMemoryPropertyFlagBits usage);
    static VkMemoryPropertyFlagBits to_vk(MemoryProperty usage);

    template<typename EG, typename VK>
    static std::vector<EG> eg_vector_from_vk_flags(VK flags){
        static_assert(sizeof(VK) * 8 == 32, "Only 32 bits flags are supported");
        std::vector<EG> result;
        uint32_t bit = 0;
        while (bit < 32){
            uint32_t flag = flags & (1u << bit);
            if (flag) {
                result.emplace_back(to_eg(static_cast<VK>(flag)));
            }

            bit++;
        }
        return result;
    }

    template<typename VK, typename EG>
    static VK to_vk_flags(const std::vector<EG>& elements) {
        VK flags = 0;
        for (auto &element : elements) {
            flags |= to_vk(element);
        }
        return flags;
    }

    template<typename EG, typename VK>
    static std::vector<EG> to_eg_vector(const std::vector<VK>& elements){
        std::vector<EG> result;
        result.reserve(elements.size());
        for (auto& element : elements){
            result.emplace_back(to_eg(element));
        }
        return result;
    }

    template<typename VK, typename EG>
    static std::vector<VK> to_vk_vector(const std::vector<EG>& elements){
        std::vector<VK> result;
        result.reserve(elements.size());
        for (auto& element : elements){
            result.emplace_back(to_vk(element));
        }
        return result;
    }

};

EG_END

#endif //EAGLE_VULKANCONVERTER_H
