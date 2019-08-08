//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_RENDERABLE_H
#define EAGLE_RENDERABLE_H

#include "eagle/engine/EngineCore.h"
#include "Mesh.h"
#include "Material.h"


EG_ENGINE_BEGIN

struct Renderable {

    Renderable(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) :
        mesh(mesh), material(material) {

    }

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;

};

EG_ENGINE_END

#endif //EAGLE_RENDERABLE_H
