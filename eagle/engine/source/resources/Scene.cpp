//
// Created by Novak on 30/07/2019.
//

#include <eagle/engine/resources/Scene.h>
#include <eagle/engine/resources/Material.h>
#include <eagle/engine/resources/Mesh.h>
#include <eagle/engine/components/Transform.h>
#include <eagle/engine/components/Renderable.h>

EG_ENGINE_BEGIN

Scene::Scene(const std::string& name):
    Resource(name),
    m_light(),
    m_entities(std::make_shared<EntityManager>()) {
}

Scene::~Scene() {

}

void Scene::update() {

}

void Scene::render(Scope<CommandBuffer>& commandBuffer, const Reference<Camera>& camera) {
    commandBuffer->begin_render_pass(camera->render_target());

    for (auto e : m_entities->entities_with_components<Transform, Renderable>()){

        ComponentHandle<Renderable> renderer = e->component<Renderable>();
        if (!renderer->material || !renderer->mesh)
            return;

        ComponentHandle<Transform> transform = e->component<Transform>();
        renderer->material->bind(commandBuffer, camera->descriptor(), m_light.descriptor());
        renderer->mesh->draw(commandBuffer, renderer->material->shader_module()->shader(), transform->matrix());
    }

    commandBuffer->end_render_pass();

}


EG_ENGINE_END


