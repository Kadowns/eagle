//
// Created by Ricardo on 04/01/21.
//

#ifndef EAGLE_TRIANGLEAPP_H
#define EAGLE_TRIANGLEAPP_H

#include <eagle/eagle.h>


class ComputeApplication : public eagle::ApplicationDelegate {
public:
    ComputeApplication();
    ~ComputeApplication() override;

    void init() override;

    void step() override;
private:

    void submit_compute();

    void submit_graphics();

private:
    eagle::RenderContext* m_renderContext = nullptr;
    eagle::EventListener m_listener;
    std::shared_ptr<eagle::Fence> m_framesInFlight;
    std::shared_ptr<eagle::Semaphore> m_frameAvailable;
    std::shared_ptr<eagle::Semaphore> m_renderFinished;
    std::shared_ptr<eagle::Semaphore> m_computeFinished;
    std::shared_ptr<eagle::Shader> m_graphicsShader;
    std::shared_ptr<eagle::Shader> m_computeShader;
    std::shared_ptr<eagle::Texture> m_computeTexture;
    std::shared_ptr<eagle::StorageBuffer> m_storageBuffer;
    std::shared_ptr<eagle::DescriptorSet> m_graphicsDescriptorSet;
    std::shared_ptr<eagle::DescriptorSet> m_computeDescriptorSet;
    std::shared_ptr<eagle::CommandBuffer> m_graphicsCommandBuffer;
    std::shared_ptr<eagle::CommandBuffer> m_computeCommandBuffer;

    eagle::Timer m_timer;
};


#endif //EAGLE_TRIANGLEAPP_H
