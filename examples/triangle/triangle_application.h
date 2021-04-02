//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>

struct Transform {

    Transform() : position{0, 0, 0}, scale{1, 1, 1}, rotation{0, 0, 0, 1} {}

    Transform(float position): position{position, position, position}, scale{1, 1, 1}, rotation{0, 0, 0, 1} {}

    float position[3];
    float scale[3];
    float rotation[4];
};


class TriangleApplication : public eagle::ApplicationDelegate {
public:
    TriangleApplication();
    ~TriangleApplication() override = default;

    void init() override;

    void step() override;

    void destroy() override;

    bool receive(const eagle::OnMouseMove& ev);

private:

    void bake_command_buffers();

private:
    eagle::TypedStackAllocator m_stackAllocator;
    eagle::PoolAllocator<Transform> m_poolAllocator;
    eagle::RenderingContext* m_renderingContext = nullptr;
    eagle::EventListener m_listener;
    std::weak_ptr<eagle::Shader> m_shader;
    std::weak_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::weak_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_primaryCommandBuffer;
    std::weak_ptr<eagle::CommandBuffer> m_secondaryCommandBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
