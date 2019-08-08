//
// Created by Novak on 16/07/2019.
//

#include <eagle/renderer/Shader.h>
#include <eagle/engine/SceneLayer.h>
#include "eagle/engine/SceneLayer.h"
#include "eagle/engine/graphics/Transform.h"
#include "eagle/engine/graphics/Renderable.h"
#include "eagle/engine/graphics/Material.h"


EG_ENGINE_BEGIN

void SceneLayer::handle_attach() {

//    m_dispatcher.add_listener<ViewportChangedEvent>([&](Event& e){
//        return handle_viewport_changed(*(ViewportChangedEvent*)&e);
//    });
//
//    auto window = Application::instance().get_window();
//
//    sceneViewport.x = 0;
//    sceneViewport.y = 0;
//    sceneViewport.width = window->get_width();
//    sceneViewport.height = window->get_height();
//
//
//    m_renderingContext = window->get_rendering_context();
//
//    m_entities = std::make_shared<entityx::EntityManager>(m_events);
//
//    VertexLayout vertexLayout(5, {
//        EG_FORMAT_R32G32B32_SFLOAT,
//        EG_FORMAT_R32G32_SFLOAT
//    });
//
//    ShaderPipelineInfo pipelineInfo(vertexLayout);
//    pipelineInfo.blendEnable = false;
//    pipelineInfo.dynamicStates = true;
//    pipelineInfo.depthTesting = true;
//
//    auto shader = m_renderingContext->create_shader(ProjectRoot + "shaders/shader.vert",
//                                                      ProjectRoot + "shaders/shader.frag", pipelineInfo);
//    std::vector<float> vertices  = {
//            1.0f, -1.0f, 1.0f,      0.0f, 1.0f,  //0.0f, 0.0f, 1.0f,      // 0
//            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //0.0f, 0.0f, 1.0f,      // 1
//            -1.0f, 1.0f, 1.0f,      1.0f, 0.0f,  //0.0f, 0.0f, 1.0f,      // 2
//            -1.0f, -1.0f, 1.0f,     1.0f, 1.0f,  //0.0f, 0.0f, 1.0f,      // 3
//
//            1.0f, 1.0f, -1.0f,      0.0f, 0.0f,  //0.0f, 0.0f, -1.0f,     // 4
//            1.0f, -1.0f, -1.0f,     0.0f, 1.0f,  //0.0f, 0.0f, -1.0f,     // 5
//            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //0.0f, 0.0f, -1.0f,     // 6
//            -1.0f, 1.0f, -1.0f,     1.0f, 0.0f,  //0.0f, 0.0f, -1.0f,     // 7
//
//            -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,  //-1.0f, 0.0f, 0.0f,     // 8
//            -1.0f, 1.0f, -1.0f,     0.0f, 1.0f,  //-1.0f, 0.0f, 0.0f,     // 9
//            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //-1.0f, 0.0f, 0.0f,     // 10
//            -1.0f, -1.0f, 1.0f,     1.0f, 0.0f,  //-1.0f, 0.0f, 0.0f,     // 11
//
//            1.0f, -1.0f, 1.0f,      1.0f, 0.0f,  //1.0f, 0.0f, 0.0f,      // 12
//            1.0f, -1.0f, -1.0f,     1.0f, 1.0f,  //1.0f, 0.0f, 0.0f,      // 13
//            1.0f, 1.0f, -1.0f,      0.0f, 1.0f,  //1.0f, 0.0f, 0.0f,      // 14
//            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //1.0f, 0.0f, 0.0f,      // 15
//
//            1.0f, -1.0f, -1.0f,     0.0f, 1.0f,  //0.0f, -1.0f, 0.0f,     // 16
//            1.0f, -1.0f, 1.0f,      0.0f, 0.0f,  //0.0f, -1.0f, 0.0f,     // 17
//            -1.0f, -1.0f, 1.0f,     1.0f, 0.0f,  //0.0f, -1.0f, 0.0f,     // 18
//            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //0.0f, -1.0f, 0.0f,     // 19
//
//            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //0.0f, 1.0f, 0.0f,      // 20
//            1.0f, 1.0f, -1.0f,      0.0f, 1.0f,  //0.0f, 1.0f, 0.0f,      // 21
//            -1.0f, 1.0f, -1.0f,     1.0f, 1.0f,  //0.0f, 1.0f, 0.0f,      // 22
//            -1.0f, 1.0f, 1.0f,      1.0f, 0.0f,  //0.0f, 1.0f, 0.0f,      // 23
//
//    };
//
//    std::vector<uint16_t> indices = {
//            0, 1, 2, 0, 2, 3,
//            4, 5, 6, 4, 6, 7,
//            8, 9, 10, 8, 10, 11,
//            12, 13, 14, 12, 14, 15,
//            16, 17, 18, 16, 18, 19,
//            20, 21, 22, 20, 22, 23
//    };
//
//    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(m_renderingContext, vertexLayout, vertices.data(), indices.data(), 24, indices.size());
//
//
//    Texture2DCreateInfo textureInfo = load_texture("textures/box.png");
//
//    auto texture = m_renderingContext->create_texture_2d(textureInfo);
//
//    std::vector<Material::MaterialItem> materialItems = {
//            {"_MainTex", texture.lock()->get_image()}
//    };
//
//    std::shared_ptr<Material> material = std::make_shared<Material>(shader.lock(), materialItems);
//    m_scene = std::make_unique<Scene>(shader.lock());
//
//    for (int i = 0; i < 10; i++) {
//        auto cube = m_entities->create();
//        cube.assign<Transform>(glm::vec3((i - 5) * 2, 0, -10), glm::quat(glm::vec3(0)), glm::vec3(1));
//        cube.assign<Renderable>(mesh, material);
//    }
//    Application::instance().event_emplace_back(std::make_shared<EntitiesReadyEvent>(m_entities));

}

void SceneLayer::handle_update() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

//
//    quad.rotation *= Math::Quaternion(glm::radians(Math::Vec3(25.0f, 15.0f, 7.5f) * 0.016f));
//    glm::normalize(quad.rotation);
//
//    quad.model *= glm::mat4_cast(quad.rotation);
//
//    camera.vp = camera.proj * camera.view;
//    RenderingContext::uniform_buffer_flush(camera.vpBuffer.lock(), &camera.vp);

}

void SceneLayer::handle_draw() {


//    m_scene->render();
//    m_renderingContext->set_viewport(sceneViewport.width, sceneViewport.height, sceneViewport.x, sceneViewport.y, 0.0f, 1.0f);
//    m_renderingContext->set_scissor(sceneViewport.width, sceneViewport.height, sceneViewport.x, sceneViewport.y);
//    m_entities->each<Transform, Renderable>([&](entityx::Entity e, Transform& transform, Renderable& renderer){
//        renderer.material->bind();
//        renderer.mesh->draw(m_renderingContext, renderer.material->shader(), transform.matrix());
//    });
}

void SceneLayer::handle_deattach() {

}

void SceneLayer::handle_event(Event &e) {
    m_dispatcher.dispatch(e);
}

//bool SceneLayer::handle_viewport_changed(ViewportChangedEvent &viewport) {
//    sceneViewport.x = viewport.x;
//    sceneViewport.y = viewport.y;
//    sceneViewport.width = viewport.w;
//    sceneViewport.height = viewport.h;
//    return false;
//}

EG_ENGINE_END