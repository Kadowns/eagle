//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_MESH_H
#define EAGLE_MESH_H

#include "eagle/engine/EngineCore.h"

EG_ENGINE_BEGIN

class Mesh {
public:

    Mesh(std::shared_ptr<RenderingContext> &renderingContext, const VertexLayout &layout, void *vertices,
         void *indices, size_t verticesCount, size_t indicesCount);
    ~Mesh();

    void draw(std::shared_ptr<RenderingContext> &renderingContext, const std::shared_ptr<Shader> &shader,
              glm::mat4 &transform) const;

private:
    std::weak_ptr<VertexBuffer> m_vertexBuffer;
    std::weak_ptr<IndexBuffer> m_indexBuffer;
};

EG_ENGINE_END


#endif //EAGLE_MESH_H
