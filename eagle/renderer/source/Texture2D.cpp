//
// Created by Novak on 19/06/2019.
//

#include "eagle/renderer/Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

_EAGLE_BEGIN

Texture2DCreateInfo Texture2D::load_texture(const std::string& path) {
    Texture2DCreateInfo createInfo = {};
    createInfo.mipLevels = 1;
    createInfo.layerCount = 1;

    Pixel* pixels = stbi_load((PROJECT_ROOT + path).c_str(), &createInfo.width, &createInfo.height, &createInfo.channels, STBI_rgb_alpha);

    size_t textureSize = createInfo.width * createInfo.height * 4;

    createInfo.pixels = std::vector<Pixel>(pixels, pixels + textureSize);

    stbi_image_free(pixels);
    return createInfo;
}

_EAGLE_END


