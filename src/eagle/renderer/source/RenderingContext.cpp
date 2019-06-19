//
// Created by Novak on 06/06/2019.
//

#include <memory>
#include "eagle/renderer/RenderingContext.h"


_EAGLE_BEGIN

std::unique_ptr<RenderingContext> RenderingContext::m_currentRenderer;

std::weak_ptr<Shader>
RenderingContext::create_shader(const std::string &vertFilePath, const std::string &fragFilePath) {
    return m_currentRenderer->handle_create_shader(vertFilePath, fragFilePath);
}

std::weak_ptr<VertexBuffer>
RenderingContext::create_vertex_buffer(std::vector<float> &vertices, size_t stride) {
    return m_currentRenderer->handle_create_vertex_buffer(vertices, stride);
}

std::weak_ptr<IndexBuffer>
RenderingContext::create_index_buffer(std::vector<uint32_t> &indices) {
    return m_currentRenderer->handle_create_index_buffer(indices);
}

std::weak_ptr<UniformBuffer>
RenderingContext::create_uniform_buffer(const ShaderItemLayout &layout) {
    return m_currentRenderer->handle_create_uniform_buffer(layout);
}

std::weak_ptr<DescriptorSet>
RenderingContext::create_descriptor_set(std::shared_ptr<Shader> shader,
                                        const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers) {
    return m_currentRenderer->handle_create_descriptor_set(shader, uniformBuffers);
}

void
RenderingContext::draw_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer) {
    m_currentRenderer->handle_draw_vertex_buffer(vertexBuffer);
}

void
RenderingContext::bind_shader(std::shared_ptr<Shader> shader) {
    m_currentRenderer->handle_bind_shader(shader);
}

void
RenderingContext::draw_indexed_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer,
                                             std::shared_ptr<IndexBuffer> indexBuffer) {
    m_currentRenderer->handle_draw_indexed_vertex_buffer(vertexBuffer, indexBuffer);
}

void
RenderingContext::bind_descriptor_set(std::shared_ptr<DescriptorSet> descriptorSet) {
    m_currentRenderer->handle_bind_descriptor_set(descriptorSet);
}

void RenderingContext::uniform_buffer_update_data(std::shared_ptr<UniformBuffer> uniformBuffer, void *data) {
    m_currentRenderer->handle_uniform_buffer_update_data(uniformBuffer, data);
}

_EAGLE_END


