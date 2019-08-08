//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_TEXTURE2D_H
#define EAGLE_TEXTURE2D_H

#include <memory>
#include <vector>

#include "eagle/core/Core.h"
#include "RenderingCore.h"
#include "Image.h"

_EAGLE_BEGIN

using Pixel = unsigned char;

struct Texture2DCreateInfo{
    int width, height;
    int channels;
    int mipLevels, layerCount;
    EG_FORMAT format;
    std::vector<Pixel> pixels;
};

class Texture2D {

public:
    explicit Texture2D(Texture2DCreateInfo textureInfo):
        m_channels(textureInfo.channels),
        m_mipLevels(textureInfo.mipLevels),
        m_layerCount(textureInfo.layerCount),
        m_format(textureInfo.format),
        m_pixels(std::move(textureInfo.pixels)){}
    virtual ~Texture2D() = default;
    inline const int get_mip_levels()             const { return m_mipLevels;   }
    inline const int get_layer_count()            const { return m_layerCount;  }
    inline const int get_channels()               const { return m_channels;    }
    inline const std::vector<Pixel>& get_pixels() const { return m_pixels;      }


    virtual std::weak_ptr<Image> get_image() = 0;
    virtual void upload_pixel_data() = 0;


protected:
    int m_channels;
    int m_mipLevels, m_layerCount;
    EG_FORMAT m_format;
    std::vector<Pixel> m_pixels;

};

_EAGLE_END

#endif //EAGLE_TEXTURE2D_H
