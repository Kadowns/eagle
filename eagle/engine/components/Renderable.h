//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_RENDERABLE_H
#define EAGLE_RENDERABLE_H

#include "../EngineCore.h"
#include "../resources/Mesh.h"
#include "../resources/Material.h"


EG_ENGINE_BEGIN

struct Renderable {

    Renderable() : mesh(-1), material(-1) {}
    Renderable(ResourceHandle<Mesh> mesh, ResourceHandle<Material> material) :
        mesh(mesh), material(material) {

    }

    ResourceHandle<Mesh> mesh;
    ResourceHandle<Material> material;

};

EG_ENGINE_END

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive &ar, Eagle::Engine::Renderable &renderable, const unsigned int version) {
    ar & renderable.mesh;
    ar & renderable.material;
}

}
}

#endif //EAGLE_RENDERABLE_H
