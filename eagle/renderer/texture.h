//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_TEXTURE_H
#define EAGLE_TEXTURE_H

#include <utility>
#include <vector>

#include "eagle/core_global_definitions.h"
#include "renderer_global_definitions.h"
#include "image.h"

namespace eagle {

struct TextureCreateInfo {
    ImageCreateInfo imageCreateInfo;
    Filter filter = Filter::LINEAR;
};

class Texture : public Descriptor {
public:
    explicit Texture(TextureCreateInfo createInfo) :
        m_createInfo(std::move(createInfo)) {}
    virtual ~Texture() = default;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    virtual std::shared_ptr<Image> image() const = 0;

protected:
    TextureCreateInfo m_createInfo;
};

}

#endif //EAGLE_TEXTURE_H
