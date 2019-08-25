//
// Created by Novak on 30/07/2019.
//

#include <eagle/engine/resources/Material.h>
#include <eagle/engine/resources/Mesh.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>
#include <eagle/editor/Serializer.h>
#include "eagle/engine/Scene.h"

EG_ENGINE_BEGIN

Scene::Scene():
    //m_camera(45.0f, 1280, 720, 0.1f, 1000.0f, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0), glm::vec3(0.0f, 0.0f, 1.0f)),
    m_renderingContext(Application::instance().get_window()->get_rendering_context()) {

}

Scene::~Scene() {

}


void Scene::init() {

    auto window = Application::instance().get_window();

    sceneViewport.x = 0;
    sceneViewport.y = 0;
    sceneViewport.width = window->get_width();
    sceneViewport.height = window->get_height();

    m_entities = std::make_shared<entityx::EntityManager>(m_events);

    Serializer serializer(ProjectRoot + "/scene.txt");

    serializer.load(*this);
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


