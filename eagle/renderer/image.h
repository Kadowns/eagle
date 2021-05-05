//
// Created by Novak on 25/06/2019.
//

#ifndef EAGLE_IMAGE_H
#define EAGLE_IMAGE_H

#include "descriptor_item.h"
#include "renderer_global_definitions.h"

namespace eagle {

struct ImageCreateInfo {
    int32_t width, height;
    uint32_t mipLevels = 1, arrayLayers = 1;
    Format format = Format::UNDEFINED;
    ImageTiling tiling = ImageTiling::OPTIMAL;
    ImageLayout layout = ImageLayout::UNDEFINED;
    std::vector<uint8_t> bufferData;
    std::vector<ImageUsage> usages;
    std::vector<MemoryProperty> memoryProperties = {MemoryProperty::DEVICE_LOCAL};
    std::vector<ImageAspect> aspects;
};

class Image : public DescriptorItem {
public:

    Image(const ImageCreateInfo& createInfo) :
        m_createInfo(createInfo) {}

    virtual ~Image() = default;

    inline uint32_t width() const { return m_createInfo.width; }
    inline uint32_t height() const { return m_createInfo.height; }
    inline uint32_t mip_levels() const { return m_createInfo.mipLevels; }
    inline uint32_t array_layers() const { return m_createInfo.arrayLayers; }
    inline Format format() const { return m_createInfo.format; }
    inline ImageTiling tiling() const { return m_createInfo.tiling; }
    inline ImageLayout layout() const { return m_createInfo.layout; }
    inline const std::vector<ImageUsage>& usages() const { return m_createInfo.usages; }
    inline const std::vector<MemoryProperty>& memory_properties() const { return m_createInfo.memoryProperties; }
    inline const std::vector<ImageAspect>& aspects() const { return m_createInfo.aspects; }
    inline const std::vector<uint8_t>& data() const { return m_createInfo.bufferData; }
    inline void resize(uint32_t width, uint32_t height) {
        m_createInfo.width = width;
        m_createInfo.height = height;
        on_resize();
    }

protected:
    virtual void on_resize() = 0;

protected:
    ImageCreateInfo m_createInfo;
};

}

#endif //EAGLE_IMAGE_H
