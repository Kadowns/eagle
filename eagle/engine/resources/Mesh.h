//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_MESH_H
#define EAGLE_MESH_H

#include "../EngineCore.h"
#include "ResourcesPool.h"

EG_ENGINE_BEGIN

class Mesh : public Resource<Mesh> {
public:

    Mesh(Reference <RenderingContext> &renderingContext, const VertexLayout &layout, void *vertices, void *indices,
         size_t verticesCount, size_t indicesCount, const std::string &name = "Mesh");
    ~Mesh();

    void draw(Reference<RenderingContext> &renderingContext, const Reference<Shader> &shader,
              glm::mat4 &transform) const;

private:
    Handle<VertexBuffer> m_vertexBuffer;
    Handle<IndexBuffer> m_indexBuffer;
};

EG_ENGINE_END


#endif //EAGLE_MESH_H
