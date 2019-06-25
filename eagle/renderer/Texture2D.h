//
// Created by Novak on 19/06/2019.
//

#ifndef EAGLE_TEXTURE2D_H
#define EAGLE_TEXTURE2D_H

#include "eagle/core/Core.h"
#include "Image.h"
#include <memory>
#include <vector>

_EAGLE_BEGIN

using Pixel = unsigned char;

struct Texture2DCreateInfo{
    int width, height, channels;
    int mipLevels, layerCount;
    std::vector<Pixel> pixels;
};

class Texture2D {

public:
    explicit Texture2D(Texture2DCreateInfo textureInfo) : m_textureInfo(std::move(textureInfo)){}
    virtual ~Texture2D() = default;
    inline const int get_width()                  const { return m_textureInfo.width;       }
    inline const int get_height()                 const { return m_textureInfo.height;      }
    inline const int get_mip_levels()             const { return m_textureInfo.mipLevels;   }
    inline const int get_layer_count()            const { return m_textureInfo.layerCount;  }
    inline const int get_channels()               const { return m_textureInfo.channels;    }
    inline const std::vector<Pixel>& get_pixels() const { return m_textureInfo.pixels;      }

    inline virtual std::weak_ptr<Image> get_image() = 0;
    virtual void upload_pixel_data() = 0;

    static Texture2DCreateInfo load_texture(const std::string& path);

protected:

    Texture2DCreateInfo m_textureInfo;

};

_EAGLE_END

#endif //EAGLE_TEXTURE2D_H
