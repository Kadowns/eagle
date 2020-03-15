//
// Created by Novak on 19/02/2020.
//

#include <eagle/engine/resources/Texture.h>

#include <stb_image.h>

EG_ENGINE_BEGIN

Texture2DCreateInfo TextureImporter::load_texture(const std::string &path) {
    Texture2DCreateInfo createInfo = {};
    createInfo.mipLevels = 1;
    createInfo.layerCount = 1;

    Pixel* pixels = stbi_load(path.c_str(), &createInfo.width, &createInfo.height, &createInfo.channels, STBI_rgb_alpha);


    size_t textureSize = createInfo.width * createInfo.height * createInfo.channels;
    Format format;

    if (createInfo.channels == 4){
        format = Format::R8G8B8A8_UNORM;
    }
    else if (createInfo.channels == 3) {
        format = Format::R8G8B8_UNORM;
    }
    else if (createInfo.channels == 2) {
        format = Format::R8G8_UNORM;
    }
    else if (createInfo.channels == 1) {
        format = Format::R8_UNORM;
    }
    else{
            throw std::runtime_error("Unsupported number of channels on texture!");

    }

    createInfo.filter = Filter::LINEAR;

    createInfo.format = format;
    createInfo.pixels = std::vector<Pixel>(pixels, pixels + textureSize);

    stbi_image_free(pixels);
    return createInfo;
}

EG_ENGINE_END