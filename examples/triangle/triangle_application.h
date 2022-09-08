//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>

class TriangleApplication : public eagle::ApplicationDelegate {
private:
    struct Mat2 {
        Mat2();
        explicit Mat2(float rotation);

        float data[2][2];
    };
public:
    TriangleApplication();
    ~TriangleApplication() override;

    void init() override;

    void step() override;
private:
    eagle::RenderContext* m_renderContext = nullptr;
    eagle::EventListener m_listener;
    eagle::Timer m_timer;
    std::shared_ptr<eagle::Fence> m_framesInFlight;
    std::shared_ptr<eagle::Semaphore> m_frameAvailable;
    std::shared_ptr<eagle::Semaphore> m_renderFinished;
    std::shared_ptr<eagle::Shader> m_shader;
    std::shared_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::shared_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::shared_ptr<eagle::UniformBuffer> m_uniformBuffer;
    std::shared_ptr<eagle::DescriptorSet> m_descriptorSet;
    std::shared_ptr<eagle::CommandBuffer> m_commandBuffer;
};


#endif //EAGLE_TRIANGLEAPP_H
