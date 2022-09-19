//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_DESCRIPTORSET_H
#define EAGLE_DESCRIPTORSET_H

#include <vector>

#include <eagle/core_global_definitions.h>
#include <eagle/renderer/uniform_buffer.h>
#include <eagle/renderer/descriptor_set_layout.h>

namespace eagle {

struct DescriptorInfo {
    Descriptor* descriptor = nullptr;
    uint32_t binding = ~0u;
};

struct BufferDescriptorInfo : DescriptorInfo {
    size_t size = ~0u;
    size_t offset = 0;
};

struct ImageDescriptorInfo : DescriptorInfo {
    ImageLayout layout;
};

struct DescriptorSetCreateInfo {
    DescriptorSetLayout* layout;
    std::vector<BufferDescriptorInfo> bufferDescriptors;
    std::vector<ImageDescriptorInfo> imageDescriptors;
};

class DescriptorSet {
public:
    explicit DescriptorSet(DescriptorSetCreateInfo info) : m_createInfo(std::move(info)) {}
    virtual ~DescriptorSet() = default;
    virtual void update() = 0;
    virtual Descriptor* operator[](uint32_t binding) = 0;
    size_t size() const { return m_createInfo.bufferDescriptors.size() + m_createInfo.imageDescriptors.size(); }
protected:
    DescriptorSetCreateInfo m_createInfo;

};

}

#endif //EAGLE_DESCRIPTORSET_H
