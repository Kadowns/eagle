//
// Created by Novak on 09/08/2019.
//

#ifndef EAGLE_SERIALIZER_H
#define EAGLE_SERIALIZER_H

#include "GlobalDefs.h"
#include <eagle/engine/resources/Scene.h>

EG_ENGINE_BEGIN

class Serializer {

public:
    Serializer(const std::string& path);

    void load(Scene &scene);
    void save(const Scene &scene);

private:
    std::string m_path;

};

EG_ENGINE_END

#endif //EAGLE_SERIALIZER_H
