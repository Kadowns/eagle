//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>

struct Color {
    union {
        float data[4];
        struct {
            float r, g, b, a;
        };
    };

};

class TriangleApplication : public eagle::ApplicationDelegate {
public:
    TriangleApplication();
    ~TriangleApplication() override;

    void init() override;

    void step() override;
private:
    eagle::RenderContext* m_renderContext = nullptr;
    eagle::EventListener m_listener;
    std::shared_ptr<eagle::Fence> m_framesInFlight;
    std::shared_ptr<eagle::Semaphore> m_frameAvailable;
    std::shared_ptr<eagle::Semaphore> m_renderFinished;
    std::shared_ptr<eagle::Shader> m_shader;
    std::shared_ptr<eagle::VertexBuffer> m_vertexBuffer;
    std::shared_ptr<eagle::IndexBuffer> m_indexBuffer;
    std::shared_ptr<eagle::UniformBuffer> m_uniformBuffer;
    std::shared_ptr<eagle::DescriptorSetLayout> m_descriptorSetLayout;
    std::shared_ptr<eagle::DescriptorSet> m_descriptorSet;
    std::shared_ptr<eagle::CommandBuffer> m_commandBuffer;

    Color m_color;
    eagle::Timer m_timer;
};


#endif //EAGLE_TRIANGLEAPP_H
