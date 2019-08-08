//
// Created by Novak on 30/07/2019.
//

#include "eagle/engine/graphics/Mesh.h"

EG_ENGINE_BEGIN

Mesh::Mesh(std::shared_ptr<RenderingContext> &renderingContext, const VertexLayout &layout, void *vertices,
           void *indices, size_t verticesCount, size_t indicesCount) {
    m_vertexBuffer = renderingContext->create_vertex_buffer(vertices, verticesCount, layout, EG_BUFFER_USAGE::CONSTANT);
    m_indexBuffer = renderingContext->create_index_buffer(indices, indicesCount, INDEX_BUFFER_TYPE::UINT_16, EG_BUFFER_USAGE::CONSTANT);
}

Mesh::~Mesh(){

}

void Mesh::draw(
        std::shared_ptr<RenderingContext> &renderingContext,
        const std::shared_ptr<Shader> &shader, glm::mat4 &transform) const {
    renderingContext->push_constants(shader, EG_SHADER_STAGE::VERTEX, 0, sizeof(glm::mat4), &transform);
    renderingContext->bind_vertex_buffer(m_vertexBuffer.lock());
    auto indexBuffer = m_indexBuffer.lock();
    renderingContext->bind_index_buffer(indexBuffer);
    renderingContext->draw_indexed(indexBuffer->get_indices_count(), 0, 0);
}

EG_ENGINE_END