//
// Created by Novak on 27/08/2019.
//

#include <eagle/engine/resources/SceneManager.h>
#include <eagle/engine/Serializer.h>
#include <boost/filesystem.hpp>


EG_ENGINE_BEGIN


SceneManager* SceneManager::s_instance = nullptr;

Scene& SceneManager::load(const std::string &sceneName) {
    return s_instance->load_scene(sceneName);
}

Scene& SceneManager::current_scene() {
    return s_instance->get_current_scene();
}


void SceneManager::create() {
    if (s_instance){
        throw std::runtime_error("A SceneManager already exists!");
    }
    s_instance = new SceneManager();
}

void SceneManager::destroy() {
    delete s_instance;
    s_instance = nullptr;
}

SceneManager& SceneManager::instance() {
    if (!s_instance){
        throw std::runtime_error("SceneManager was not initialized!");
    }
    SceneManager* manager = dynamic_cast<SceneManager*>(s_instance);
    if (!manager){
        throw std::runtime_error("SceneManager instance was not of type SceneManager");
    }
    return *manager;
}

void SceneManager::store(const std::string &pathName) {

    boost::filesystem::path path(pathName);

    if (!boost::filesystem::is_regular_file(path)){
        throw std::runtime_error("Invalid path name!" + pathName);
    }

    std::string name = path.filename().string();

    name = name.erase(name.find('.'));

    if (m_scenes.find(name) != m_scenes.end()){
        throw std::runtime_error("Scene with name" + name + " is already present");
    }

    ResourceID id = m_scenes.size();
    ResourcesPool::instance().store<Scene>(new Scene(name), id);
    m_scenes[name] = SceneData(ResourceHandle<Scene>(id), pathName);
}

Scene& SceneManager::load_scene(const std::string &sceneName) {

    if (m_scenes.find(sceneName) == m_scenes.end()){
        throw std::runtime_error("Scene with name " + sceneName + " does not exist!");
    }
    Serializer serializer(m_scenes[sceneName].path);
    serializer.load(*m_scenes[sceneName].scene);
    m_currentScene = m_scenes[sceneName].scene;
    return *m_currentScene;
}

Scene &SceneManager::get_current_scene() {
    return *m_currentScene;
}

EG_ENGINE_END
