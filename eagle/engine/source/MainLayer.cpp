//
// Created by Novak on 15/11/2019.
//

#include <eagle/engine/MainLayer.h>
#include <eagle/engine/Renderer.h>
#include <eagle/engine/ModelImporter.h>
#include <eagle/engine/resources/ResourcesPool.h>
#include <eagle/engine/resources/Mesh.h>
#include <eagle/engine/resources/Material.h>
#include <eagle/engine/resources/Texture.h>
#include <eagle/engine/resources/ShaderModule.h>
#include <eagle/engine/resources/SceneManager.h>

#include <eagle/engine/geometry/MeshFactory.h>


EG_ENGINE_BEGIN

void MainLayer::handle_attach() {

    Renderer::initialize(Application::instance().window());


    ResourcesPool::instance().store<Mesh>(MeshFactory::create_cube(), 0);
    ResourcesPool::instance().store<Mesh>(MeshFactory::create_quad(), 1);
    ResourcesPool::instance().store<Mesh>(MeshFactory::create_cube_sphere(), 2);

    ResourcesPool::instance().store<ShaderModule>(new ShaderModule(
            ProjectRoot + "shaders/surface.shader",
            "SurfaceShader"), 0);

    Texture2DCreateInfo defaultTexture = {};
    defaultTexture.width = 1;
    defaultTexture.height = 1;
    defaultTexture.format = Format::R8G8B8A8_UNORM;
    defaultTexture.filter = Filter::NEAREST;
    defaultTexture.mipLevels = 1;
    defaultTexture.layerCount = 1;
    defaultTexture.channels = 4;
    defaultTexture.pixels = {255, 255, 255, 255};

    ResourcesPool::instance().store<Texture>(new Texture(defaultTexture, "none"), 0);
    ResourcesPool::instance().store<Texture>(new Texture(TextureImporter::load_texture(ProjectRoot + "textures/box.png"), "box"), 1);
    ResourcesPool::instance().store<Texture>(new Texture(TextureImporter::load_texture(ProjectRoot + "textures/metal.png"), "metal"), 2);
    ResourcesPool::instance().store<Texture>(new Texture(TextureImporter::load_texture(ProjectRoot + "textures/gold.png"), "gold"), 3);

    ResourcesPool::instance().store<Material>(new Material(ResourceHandle<ShaderModule>(0), "Madeira"), 0);
    ResourceHandle<Material>(0)->set_texture("uDiffuse0", ResourceHandle<Texture>(1));
    ResourceHandle<Material>(0)->set_uniform("uMaterial", "color", glm::vec3(1));
    ResourceHandle<Material>(0)->set_uniform("uMaterial", "specularPower", 1.0f);
    ResourceHandle<Material>(0)->update();
    ResourcesPool::instance().store<Material>(new Material(ResourceHandle<ShaderModule>(0), "Metal"), 1);
    ResourceHandle<Material>(1)->set_texture("uDiffuse0", ResourceHandle<Texture>(2));
    ResourceHandle<Material>(1)->set_uniform("uMaterial", "color", glm::vec3(1, 0.5f, 0.8f));
    ResourceHandle<Material>(1)->set_uniform("uMaterial", "specularPower", 128.0f);
    ResourceHandle<Material>(1)->update();


    ModelImporter importer(ProjectRoot + "models/scene.fbx");





    SceneManager::create();
    SceneManager::instance().store(ProjectRoot + "scene.txt");

}

void MainLayer::handle_deattach() {

}

void MainLayer::handle_update() {

}

void MainLayer::handle_event(Event &e) {

}

EG_ENGINE_END


