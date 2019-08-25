//
// Created by Novak on 09/08/2019.
//

#ifndef EAGLE_SERIALIZER_H
#define EAGLE_SERIALIZER_H

#include "eagle/engine/EngineCore.h"
#include "eagle/engine/Scene.h"
#include "eagle/engine/resources/Material.h"
#include "eagle/engine/resources/Mesh.h"

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
