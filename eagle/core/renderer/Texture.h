//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_TEXTURE_H
#define EAGLE_TEXTURE_H

#include <utility>
#include <vector>

#include "eagle/core/CoreGlobalDefinitions.h"
#include "RenderingCore.h"
#include "Image.h"

EG_BEGIN

struct TextureCreateInfo {
    ImageCreateInfo imageCreateInfo;
    Filter filter = Filter::LINEAR;
};

class Texture : public DescriptorItem {
public:
    explicit Texture(TextureCreateInfo createInfo) :
        DescriptorItem(DescriptorType::TEXTURE),
        m_createInfo(std::move(createInfo)) {}
    virtual ~Texture() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual Reference<Image> image() const = 0;

protected:
    TextureCreateInfo m_createInfo;
};

EG_END

#endif //EAGLE_TEXTURE_H
