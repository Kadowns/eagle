//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/Core.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "DescriptorSet.h"
#include "Texture2D.h"

_EAGLE_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() { m_currentRenderer = std::unique_ptr<RenderingContext>(this); }
    virtual ~RenderingContext() = default;

    virtual void init(Window *window) = 0;
    virtual void begin_draw() = 0;
    virtual void end_draw() = 0;
    virtual void refresh() = 0;
    virtual void deinit() = 0;
    virtual void handle_window_resized(int width, int height) = 0;

    static std::weak_ptr<Shader> create_shader(const std::string &vertFilePath, const std::string &fragFilePath);
    static std::weak_ptr<VertexBuffer> create_vertex_buffer(std::vector<float> &vertices, size_t stride);
    static std::weak_ptr<IndexBuffer> create_index_buffer(std::vector<uint32_t>& indices);
    static std::weak_ptr<UniformBuffer> create_uniform_buffer(const ShaderItemLayout& layout);
    static std::weak_ptr<DescriptorSet> create_descriptor_set(std::shared_ptr<Shader> shader,
                                                              const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers = {},
                                                              const std::vector<std::shared_ptr<Texture2D>> &textures = {});
    static std::weak_ptr<Texture2D> create_texture_2d(const std::string& filePath);
    static void bind_shader(std::shared_ptr<Shader> shader);
    static void bind_descriptor_set(std::shared_ptr<DescriptorSet> descriptorSet);
    static void draw_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer);
    static void uniform_buffer_update_data(std::shared_ptr<UniformBuffer> uniformBuffer, void *data);
    static void draw_indexed_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer);

protected:

    static std::unique_ptr<RenderingContext> m_currentRenderer;

    virtual std::weak_ptr<Shader>
    handle_create_shader(const std::string &vertFilePath, const std::string &fragFilePath) = 0;

    virtual std::weak_ptr<VertexBuffer>
    handle_create_vertex_buffer(std::vector<float> &vertices, size_t stride) = 0;

    virtual std::weak_ptr<IndexBuffer>
    handle_create_index_buffer(std::vector<uint32_t> &indices) = 0;

    virtual std::weak_ptr<UniformBuffer>
    handle_create_uniform_buffer(const ShaderItemLayout& layout) = 0;

    virtual std::weak_ptr<DescriptorSet>
    handle_create_descriptor_set(std::shared_ptr<Shader> shader,
                                 const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers,
                                 const std::vector<std::shared_ptr<Texture2D>> &textures) = 0;

    virtual std::weak_ptr<Texture2D>
    handle_create_texture_2d(const std::string& filePath) = 0;

    virtual void
    handle_bind_shader(std::shared_ptr<Shader> shader) = 0;

    virtual void
    handle_draw_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer) = 0;

    virtual void
    handle_draw_indexed_vertex_buffer(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer) = 0;

    virtual void
    handle_uniform_buffer_update_data(std::shared_ptr<UniformBuffer> uniformBuffer, void *data) = 0;

    virtual void
    handle_bind_descriptor_set(std::shared_ptr<DescriptorSet> descriptorSet) = 0;

};

_EAGLE_END

#endif //EAGLE_RENDERINGCONTEXT_H
