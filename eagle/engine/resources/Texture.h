//
// Created by Novak on 29/08/2019.
//

#ifndef EAGLE_TEXTURE_H
#define EAGLE_TEXTURE_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/Renderer.h>
#include "ResourcesPool.h"


EG_ENGINE_BEGIN

class TextureImporter {
public:
    static Texture2DCreateInfo load_texture(const std::string& path);
};

class Texture : public Resource<Texture> {
public:
    Texture(const Texture2DCreateInfo &info, const std::string &name = "Texture") : Resource(name){
        m_textureHandle = Renderer::instance().context().create_texture_2d(info);
    }

    virtual ~Texture(){

    }

    inline Handle<Texture2D>& handle() {return m_textureHandle; }

private:

    Handle<Texture2D> m_textureHandle;

};

EG_ENGINE_END

#endif //EAGLE_TEXTURE_H
