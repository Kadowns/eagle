#ifndef EAGLE_VULKANUNIFORMBUFFERSET_H
#define EAGLE_VULKANUNIFORMBUFFERSET_H

#include "eagle/renderer/UniformBufferSet.h"
#include "eagle/renderer/vulkan/VulkanCore.h"

_EAGLE_BEGIN

class VulkanUniformBufferSet : public UniformBufferSet {
public:
    VulkanUniformBufferSet(const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers);


private:
    void create_descriptor_pool();


private:

    VkDescriptorPool m_descriptorPool;

};

_EAGLE_END

#endif //EAGLE_VULKANUNIFORMBUFFERSET_H
