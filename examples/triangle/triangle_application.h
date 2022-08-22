//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>

class TriangleApplication : public eagle::ApplicationDelegate {
public:
    TriangleApplication();
    ~TriangleApplication() override;

    void init() override;

    void step() override;
private:
    eagle::RenderingContext* m_renderingContext = nullptr;
    eagle::EventListener m_listener;
    std::shared_ptr<eagle::GPUQueue> m_renderQueue;
    std::shared_ptr<eagle::Fence> m_framesInFlight;
    std::shared_ptr<eagle::Semaphore> m_frameAvailable;
    std::shared_ptr<eagle::Semaphore> m_renderFinished;
    std::shared_ptr<eagle::Shader> m_shader;
    std::shared_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::shared_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::shared_ptr<eagle::CommandBuffer> m_commandBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
