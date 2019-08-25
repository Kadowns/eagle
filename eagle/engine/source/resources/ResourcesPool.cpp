//
// Created by Novak on 10/08/2019.
//

#include "eagle/engine/resources/ResourcesPool.h"

#include <boost/filesystem.hpp>
#include <stb_image.h>
#include <eagle/engine/resources/Mesh.h>

EG_ENGINE_BEGIN

BaseResource::Family BaseResource::s_familyCounter = 0;

ResourcesPool::~ResourcesPool() {
    for (auto& type : m_resources){
        for (auto& it : type){
            delete it.second;
        }
    }
}

//ResourceManager* ResourceManager::s_instance;
//
//ResourceManager& ResourceManager::instance() {
//    return *s_instance;
//}
//
//void ResourceManager::initialize(const std::string &path) {
//    s_instance = new ResourceManager(path);
//}
//
//ResourceManager::ResourceManager(const std::string &root) :
//    m_root(root){
//
//    boost::filesystem::recursive_directory_iterator it(root);
//    m_paths.emplace_back(it->path().parent_path());
//    for (; it != boost::filesystem::recursive_directory_iterator(); ++it){
//
//        boost::filesystem::path path = it->path();
//
//        if (boost::filesystem::is_regular_file(path)){
//            load_file(path);
//        }
//
//        m_paths.emplace_back(path);
//    }
//
//    auto pool = new entityx::Pool<Mesh>();
//
//
//
//    m_resourcesPools.push_back();
//
//}
//
//void ResourceManager::load_file(const boost::filesystem::path &path) {
//    if (is_image(path)){
//        //Texture2DHandle handle(1);
//        //m_textures[= load_texture(path);
//    }
//}
//
//bool ResourceManager::is_image(const boost::filesystem::path &path) const {
//    return path.extension().string() == "png";
//}
//
//Handle<Texture2D> ResourceManager::load_texture(const boost::filesystem::path &path) {
//
//    Texture2DCreateInfo createInfo = {};
//    createInfo.mipLevels = 1;
//    createInfo.layerCount = 1;
//
//    Eagle::Pixel* pixels = stbi_load(path.string().c_str(), &createInfo.width, &createInfo.height, &createInfo.channels, STBI_rgb_alpha);
//
//    size_t textureSize = createInfo.width * createInfo.height * createInfo.channels;
//
//    createInfo.format = Eagle::EG_FORMAT_R8G8B8A8_UNORM;
//
//    createInfo.pixels = std::vector<Eagle::Pixel>(pixels, pixels + textureSize);
//
//    stbi_image_free(pixels);
//
//    return Application::instance().get_window()->get_rendering_context()->create_texture_2d(createInfo);
//}
//

EG_ENGINE_END
