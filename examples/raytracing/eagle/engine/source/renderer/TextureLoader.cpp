//
// Created by Novak on 04/04/2020.
//

#include <eagle/engine/renderer/TextureLoader.h>
#include <stb/stb_image.h>

EG_ENGINE_BEGIN

TextureCreateInfo TextureLoader::load_pixels(const std::string &path) {

    ImageCreateInfo imageCreateInfo = {};
    int width, height, bpp;
    auto *pixels = (unsigned char *) stbi_load(path.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
    if (!pixels) {
        throw std::runtime_error("Failed to load image: " + std::string(stbi_failure_reason()));
    }

    imageCreateInfo.width = width;
    imageCreateInfo.height = height;

    int len = imageCreateInfo.width * imageCreateInfo.height * 4;
    imageCreateInfo.bufferData.resize(len);
    memcpy(imageCreateInfo.bufferData.data(), pixels, len);
    stbi_image_free(pixels);

    //todo - add other formats support
    imageCreateInfo.format = Format::R8G8B8A8_UNORM;
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.tiling = ImageTiling::LINEAR;
    imageCreateInfo.memoryProperties = {MemoryProperty::DEVICE_LOCAL};
    imageCreateInfo.aspects = {ImageAspect::COLOR};
    imageCreateInfo.usages = {ImageUsage::SAMPLED};
    imageCreateInfo.layout = ImageLayout::SHADER_READ_ONLY_OPTIMAL;

    TextureCreateInfo textureCreateInfo = {};
    textureCreateInfo.imageCreateInfo = std::move(imageCreateInfo);
    textureCreateInfo.filter = Filter::LINEAR;
    return textureCreateInfo;
}


EG_ENGINE_END


