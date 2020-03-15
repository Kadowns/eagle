//
// Created by Novak on 07/12/2019.
//

#include <eagle/engine/geometry/MeshFactory.h>


EG_ENGINE_BEGIN

Mesh* MeshFactory::create_cube(){
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generate_cube_vertices(vertices, indices, 64);
    VertexLayout vertexLayout(8, {
            Format::R32G32B32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R32G32B32_SFLOAT
    });
    return new Mesh(vertexLayout, vertices.data(), indices.data(), vertices.size(), indices.size(), "Cube");
}

void
MeshFactory::generate_normals(std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) {
    for (uint32_t i = 0; i < indices.size(); i += 3) {
        uint32_t i1 = indices[i];
        uint32_t i2 = indices[i + 1];
        uint32_t i3 = indices[i + 2];

        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;
        glm::vec3 v3 = vertices[i3].position;

        glm::vec3 side1 = v2 - v1;
        glm::vec3 side2 = v3 - v1;

        glm::vec3 normal = glm::cross(side1, side2);

        vertices[i1].normal += normal;
        vertices[i2].normal += normal;
        vertices[i3].normal += normal;
    }
    for (auto& it : vertices) {
        glm::normalize(it.normal);
    }
}


void MeshFactory::generate_cube_vertices(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices,
                                         uint32_t resolution) {

    assert(vertices.empty() && indices.empty());

    glm::vec3 directions[] = {
            glm::vec3(0.0f, 1.0f, 0.0f), //up
            glm::vec3(0.0f, -1.0f, 0.0f),//down
            glm::vec3(1.0f, 0.0f, 0.0f), //right
            glm::vec3(-1.0f, 0.0f, 0.0f),//left
            glm::vec3(0.0f, 0.0f, 1.0f), //back
            glm::vec3(0.0f, 0.0f, -1.0f),//forward
    };

    for (uint8_t face = 0; face < 6; face++) {

        glm::vec3 localUp = directions[face];
        glm::vec3 localRight = glm::vec3(localUp.y, localUp.z, localUp.x);
        glm::vec3 localFront = glm::cross(localUp, localRight);

        uint32_t faceOffset = vertices.size();

        for (uint8_t x = 0; x < resolution; x++) {
            for (uint8_t z = 0; z < resolution; z++) {
                Vertex vertex = {};
                glm::vec2 percent = glm::vec2(x, z) / (resolution - 1.0f);
                glm::vec3 pointOnUnitCube =
                        (localUp + (percent.x - 0.5f) * 2 * localRight + (percent.y - 0.5f) * 2 * localFront);
                vertex.position = pointOnUnitCube;
                vertex.uv = percent;
                vertex.normal = localUp;
                vertices.emplace_back(vertex);
            }
        }


        //indices.resize(positions.size());
        for (uint8_t z = 0; z < resolution - 1; z++) {
            for (uint8_t x = 0; x < resolution - 1; x++) {
                uint32_t zero = (x + z * resolution) + faceOffset;
                uint32_t one = ((x + 1) + z * resolution) + faceOffset;
                uint32_t two = (x + (z + 1) * resolution) + faceOffset;
                uint32_t three = ((x + 1) + (z + 1) * resolution) + faceOffset;
                uint32_t index = indices.size();
                indices.resize(index + 6);

                indices[index] = zero;
                indices[index + 1] = two;
                indices[index + 2] = three;

                indices[index + 3] = zero;
                indices[index + 4] = three;
                indices[index + 5] = one;
            }
        }
    }
}

Mesh *MeshFactory::create_quad() {
    std::vector<float> vertices  = {
            1.0f, -1.0f, 0.0f,      0.0f, 1.0f,  0.0f, 0.0f, 1.0f,      // 0
            1.0f, 1.0f, 0.0f,       0.0f, 0.0f,  0.0f, 0.0f, 1.0f,      // 1
            -1.0f, 1.0f, 0.0f,      1.0f, 0.0f,  0.0f, 0.0f, 1.0f,      // 2
            -1.0f, -1.0f, 0.0f,     1.0f, 1.0f,  0.0f, 0.0f, 1.0f,      // 3
    };

    std::vector<uint32_t> indices = {
            0, 1, 2, 0, 2, 3,
    };

    VertexLayout vertexLayout(8, {
            Format::R32G32B32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R32G32B32_SFLOAT
    });

    return new Mesh(vertexLayout, vertices.data(), indices.data(), 4, indices.size(), "Plane");
}

Mesh *MeshFactory::create_cube_sphere() {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    generate_cube_vertices(vertices, indices, 64);
    VertexLayout vertexLayout(8, {
            Format::R32G32B32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R32G32B32_SFLOAT
    });

    for (auto& v : vertices){
        v.position = glm::normalize(v.position);
        v.normal = v.position;
    }

    return new Mesh(vertexLayout, vertices.data(), indices.data(), vertices.size(), indices.size(), "Sphere");
}


EG_ENGINE_END