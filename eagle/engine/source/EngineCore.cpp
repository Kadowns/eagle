//
// Created by Novak on 26/06/2019.
//

#include "eagle/engine/EngineCore.h"

#include <stb_image.h>

EG_ENGINE_BEGIN

Eagle::Texture2DCreateInfo load_texture(const std::string& path){
    Eagle::Texture2DCreateInfo createInfo = {};
    createInfo.mipLevels = 1;
    createInfo.layerCount = 1;

    Eagle::Pixel* pixels = stbi_load((ProjectRoot + path).c_str(), &createInfo.width, &createInfo.height, &createInfo.channels, STBI_rgb_alpha);


    size_t textureSize = createInfo.width * createInfo.height * createInfo.channels;
    EG_FORMAT format;

    if (createInfo.channels == 4){
        format = Eagle::EG_FORMAT_R8G8B8A8_UNORM;
    }else{
        throw std::runtime_error("Unsupported number of channels on texture!");
    }

    createInfo.format = format;

    createInfo.pixels = std::vector<Eagle::Pixel>(pixels, pixels + textureSize);

    stbi_image_free(pixels);
    return createInfo;
}

EG_ENGINE_END