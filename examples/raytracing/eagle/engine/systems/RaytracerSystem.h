//
// Created by Novak on 09/04/2020.
//

#ifndef EAGLE_RAYTRACERSYSTEM_H
#define EAGLE_RAYTRACERSYSTEM_H

#include <eagle/engine/EngineGlobalDefinitions.h>
#include <eagle/engine/renderer/RenderMaster.h>
#include <eagle/engine/components/Camera.h>
#include <eagle/engine/components/DirectionalLight.h>
#include <eagle/engine/components/SceneData.h>
#include <eagle/engine/components/Sphere.h>
#include <eagle/engine/components/Box.h>

EG_ENGINE_BEGIN

class RaytracerSystem : public entityx::System<RaytracerSystem>, public entityx::Receiver<RaytracerSystem> {
public:
    RaytracerSystem();
    virtual ~RaytracerSystem() override;

    virtual void configure(entityx::EntityManager &entities, entityx::EventManager &events) override;
    virtual void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt) override;
    void receive(const OnCameraUpdate& ev);
    void receive(const OnLightUpdate& ev);
    void receive(const entityx::ComponentAddedEvent<Sphere>& ev);
    void receive(const entityx::ComponentAddedEvent<Box>& ev);
    void receive(const entityx::ComponentRemovedEvent<Sphere>& ev);
    void receive(const entityx::ComponentRemovedEvent<Box>& ev);
    //event bus events
    bool receive(const OnRenderingContextRecreated& ev);
    bool receive(const OnRenderContextInit& ev);
    bool receive(const OnRenderContextDeinit& ev);
    bool receive(const OnRenderFrameBegin& ev);
    bool receive(const OnRenderCommandBufferBegin& ev);
    bool receive(const OnRenderCommandBufferMainRenderPass& ev);
private:
    void init_render_target();
    void update_sphere_buffer(entityx::EntityManager &entities);
    void update_box_buffer(entityx::EntityManager &entities);

private:
    EventListener<RaytracerSystem> m_listener;
    bool m_updateSpheres;
    bool m_updateBoxes;
};


EG_ENGINE_END

#endif //EAGLE_RAYTRACERSYSTEM_H
