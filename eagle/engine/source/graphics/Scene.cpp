//
// Created by Novak on 30/07/2019.
//

#include <eagle/engine/graphics/Material.h>
#include <eagle/engine/graphics/Mesh.h>
#include <eagle/engine/graphics/Transform.h>
#include <eagle/engine/graphics/Renderable.h>
#include "eagle/engine/graphics/Scene.h"

EG_ENGINE_BEGIN

Scene::Scene():
    //m_camera(45.0f, 1280, 720, 0.1f, 1000.0f, glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0), glm::vec3(0.0f, 0.0f, 1.0f)),
    m_renderingContext(Application::instance().get_window()->get_rendering_context()) {

}

Scene::~Scene() {

}


void Scene::init(const std::shared_ptr<Camera> &camera) {

//    m_dispatcher.add_listener<ViewportChangedEvent>([&](Event& e){
//        return handle_viewport_changed(*(ViewportChangedEvent*)&e);
//    });

    auto window = Application::instance().get_window();

    sceneViewport.x = 0;
    sceneViewport.y = 0;
    sceneViewport.width = window->get_width();
    sceneViewport.height = window->get_height();


    m_entities = std::make_shared<entityx::EntityManager>(m_events);

    //m_renderTarget = m_renderingContext->create_render_target({RENDER_TARGET_ATTACHMENT::COLOR});

    VertexLayout vertexLayout(5, {
            EG_FORMAT_R32G32B32_SFLOAT,
            EG_FORMAT_R32G32_SFLOAT
    });

    DescriptorBinding worldBinding = {};
    worldBinding.binding = 0;
    worldBinding.descriptorType = EG_DESCRIPTOR_TYPE::UNIFORM_BUFFER;
    worldBinding.shaderStage = EG_SHADER_STAGE::VERTEX;

    DescriptorBinding materialBinding = {};
    materialBinding.binding = 0;
    materialBinding.descriptorType = EG_DESCRIPTOR_TYPE::IMAGE_2D;
    materialBinding.shaderStage = EG_SHADER_STAGE::FRAGMENT;

    m_materialDescriptorSetLayout = m_renderingContext->create_descriptor_set_layout({materialBinding});

    ShaderPipelineInfo pipelineInfo(vertexLayout);
    pipelineInfo.blendEnable = false;
    pipelineInfo.dynamicStates = false;
    pipelineInfo.depthTesting = true;

     m_shader = m_renderingContext->create_shader(ProjectRoot + "shaders/shader.vert",
                                                  ProjectRoot + "shaders/shader.frag",
                                                  {camera->descriptor_layout(), m_materialDescriptorSetLayout.lock()},
                                                  pipelineInfo, camera->render_target());
    std::vector<float> vertices  = {
            1.0f, -1.0f, 1.0f,      0.0f, 1.0f,  //0.0f, 0.0f, 1.0f,      // 0
            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //0.0f, 0.0f, 1.0f,      // 1
            -1.0f, 1.0f, 1.0f,      1.0f, 0.0f,  //0.0f, 0.0f, 1.0f,      // 2
            -1.0f, -1.0f, 1.0f,     1.0f, 1.0f,  //0.0f, 0.0f, 1.0f,      // 3

            1.0f, 1.0f, -1.0f,      0.0f, 0.0f,  //0.0f, 0.0f, -1.0f,     // 4
            1.0f, -1.0f, -1.0f,     0.0f, 1.0f,  //0.0f, 0.0f, -1.0f,     // 5
            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //0.0f, 0.0f, -1.0f,     // 6
            -1.0f, 1.0f, -1.0f,     1.0f, 0.0f,  //0.0f, 0.0f, -1.0f,     // 7

            -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,  //-1.0f, 0.0f, 0.0f,     // 8
            -1.0f, 1.0f, -1.0f,     0.0f, 1.0f,  //-1.0f, 0.0f, 0.0f,     // 9
            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //-1.0f, 0.0f, 0.0f,     // 10
            -1.0f, -1.0f, 1.0f,     1.0f, 0.0f,  //-1.0f, 0.0f, 0.0f,     // 11

            1.0f, -1.0f, 1.0f,      1.0f, 0.0f,  //1.0f, 0.0f, 0.0f,      // 12
            1.0f, -1.0f, -1.0f,     1.0f, 1.0f,  //1.0f, 0.0f, 0.0f,      // 13
            1.0f, 1.0f, -1.0f,      0.0f, 1.0f,  //1.0f, 0.0f, 0.0f,      // 14
            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //1.0f, 0.0f, 0.0f,      // 15

            1.0f, -1.0f, -1.0f,     0.0f, 1.0f,  //0.0f, -1.0f, 0.0f,     // 16
            1.0f, -1.0f, 1.0f,      0.0f, 0.0f,  //0.0f, -1.0f, 0.0f,     // 17
            -1.0f, -1.0f, 1.0f,     1.0f, 0.0f,  //0.0f, -1.0f, 0.0f,     // 18
            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,  //0.0f, -1.0f, 0.0f,     // 19

            1.0f, 1.0f, 1.0f,       0.0f, 0.0f,  //0.0f, 1.0f, 0.0f,      // 20
            1.0f, 1.0f, -1.0f,      0.0f, 1.0f,  //0.0f, 1.0f, 0.0f,      // 21
            -1.0f, 1.0f, -1.0f,     1.0f, 1.0f,  //0.0f, 1.0f, 0.0f,      // 22
            -1.0f, 1.0f, 1.0f,      1.0f, 0.0f,  //0.0f, 1.0f, 0.0f,      // 23

    };

    std::vector<uint16_t> indices = {
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
    };

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(m_renderingContext, vertexLayout, vertices.data(), indices.data(), 24, indices.size());


    Texture2DCreateInfo textureInfo = load_texture("textures/box.png");

    auto texture = m_renderingContext->create_texture_2d(textureInfo);

    std::vector<Material::MaterialItem> materialItems = {
            {"_MainTex", texture.lock()->get_image()}
    };

    std::shared_ptr<Material> material = std::make_shared<Material>(m_shader.lock(), m_materialDescriptorSetLayout.lock(), materialItems);

    for (int i = 0; i < 10; i++) {
        auto cube = m_entities->create();
        cube.assign<Transform>(glm::vec3((i - 5) * 2, 0, -10), glm::quat(glm::vec3(0)), glm::vec3(1));
        cube.assign<Renderable>(mesh, material);
    }
    //Application::instance().event_emplace_back(std::make_shared<EntitiesReadyEvent>(m_entities));
}

void Scene::update() {

}

void Scene::render(const std::shared_ptr<Camera> &camera) {

    m_renderingContext->begin_draw_offscreen(camera->render_target());
    m_renderingContext->bind_descriptor_sets(m_shader.lock(), camera->descriptor(), 0);
    //m_renderingContext->set_viewport(sceneViewport.width, sceneViewport.height, sceneViewport.x, sceneViewport.y, 0.0f, 1.0f);
    //m_renderingContext->set_scissor(sceneViewport.width, sceneViewport.height, sceneViewport.x, sceneViewport.y);
    m_entities->each<Transform, Renderable>([&](entityx::Entity e, Transform& transform, Renderable& renderer){
        renderer.material->bind();
        renderer.mesh->draw(m_renderingContext, renderer.material->shader(), transform.matrix());
    });
    m_renderingContext->end_draw_offscreen();
}


EG_ENGINE_END


