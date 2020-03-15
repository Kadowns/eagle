//
// Created by Novak on 30/07/2019.
//

#include "eagle/engine/resources/Mesh.h"
#include "eagle/engine/Renderer.h"

EG_ENGINE_BEGIN

Mesh::Mesh(const VertexLayout &layout, void *vertices, void *indices, size_t verticesCount, size_t indicesCount,
           const std::string &name) : Resource(name) {

    RenderingContext& context = Renderer::instance().context();
    m_vertexBuffer = context.create_vertex_buffer(vertices, verticesCount, layout, BufferUsage::CONSTANT);
    m_indexBuffer = context.create_index_buffer(indices, indicesCount, IndexBufferType::UINT_32, BufferUsage::CONSTANT);
}

Mesh::~Mesh(){

}

void Mesh::draw(Scope<CommandBuffer> &commandBuffer, const Reference<Shader> &shader, glm::mat4 &transform) const {

    glm::mat4 inverse = glm::inverse(glm::transpose(transform));
    commandBuffer->push_constants(shader, ShaderStage::VERTEX, 0, sizeof(glm::mat4), &transform);
    commandBuffer->push_constants(shader, ShaderStage::VERTEX, sizeof(glm::mat4), sizeof(glm::mat4), &inverse);
    commandBuffer->bind_vertex_buffer(m_vertexBuffer.lock());
    auto indexBuffer = m_indexBuffer.lock();
    commandBuffer->bind_index_buffer(indexBuffer);
    commandBuffer->draw_indexed(indexBuffer->get_indices_count(), 0, 0);
}

EG_ENGINE_END