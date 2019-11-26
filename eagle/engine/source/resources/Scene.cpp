//
// Created by Novak on 30/07/2019.
//

#include <eagle/engine/resources/Material.h>
#include <eagle/engine/resources/Mesh.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>
#include <eagle/engine/resources/Scene.h>

EG_ENGINE_BEGIN

Scene::Scene(const std::string& name):
    m_name(name),
    m_entities(std::make_shared<entityx::EntityManager>(m_events)),
    m_renderingContext(Application::instance().get_window()->get_rendering_context()) {

}

Scene::~Scene() {

}

void Scene::update() {

}

void Scene::render(const Reference<Camera> &camera) {

    m_renderingContext->begin_draw_offscreen(camera->render_target());

    m_entities->each<Transform, Renderable>([&](entityx::Entity e, Transform& transform, Renderable& renderer){

        if (!renderer.material || !renderer.mesh)
            return;

        renderer.material->bind(camera);
        renderer.mesh->draw(m_renderingContext, renderer.material->shader(), transform.matrix());
    });
    m_renderingContext->end_draw_offscreen();
}


EG_ENGINE_END


