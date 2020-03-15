//
// Created by Novak on 07/12/2019.
//

#ifndef EAGLE_MESHFACTORY_H
#define EAGLE_MESHFACTORY_H

#include <eagle/engine/GlobalDefs.h>
#include <eagle/engine/resources/Mesh.h>
#include "Vertex.h"


EG_ENGINE_BEGIN

class MeshFactory {

public:

    static Mesh* create_cube();
    static Mesh* create_quad();
    static Mesh* create_cube_sphere();

    static void generate_normals(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
    static void generate_cube_vertices(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
                                       uint32_t resolution);


};

EG_ENGINE_END

#endif //EAGLE_MESHFACTORY_H
