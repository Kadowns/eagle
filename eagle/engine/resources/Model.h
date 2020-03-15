//
// Created by Novak on 19/02/2020.
//

#ifndef EAGLE_MODEL_H
#define EAGLE_MODEL_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/resources/ResourcesPool.h>
#include "Mesh.h"
#include "Material.h"

EG_ENGINE_BEGIN

class Model : Resource<Model> {
public:
    void draw(Scope <Eagle::CommandBuffer> &commandBuffer, const Reference <DescriptorSet> &camera,
              const Reference <DescriptorSet> &world);

private:
    std::vector<std::pair<ResourceHandle<Mesh>, ResourceHandle<Material>>> m_meshes;

};


EG_ENGINE_END

#endif //EAGLE_MODEL_H
