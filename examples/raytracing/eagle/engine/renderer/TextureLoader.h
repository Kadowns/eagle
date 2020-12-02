//
// Created by Novak on 04/04/2020.
//

#ifndef EAGLE_TEXTURELOADER_H
#define EAGLE_TEXTURELOADER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

class TextureLoader {
public:
    static TextureCreateInfo load_pixels(const std::string &path);
};

EG_ENGINE_END

#endif //EAGLE_TEXTURELOADER_H
