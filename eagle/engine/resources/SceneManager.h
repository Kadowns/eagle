//
// Created by Novak on 27/08/2019.
//

#ifndef EAGLE_SCENEMANAGER_H
#define EAGLE_SCENEMANAGER_H

#include "eagle/engine/GlobalDefs.h"
#include "eagle/engine/resources/Scene.h"

EG_ENGINE_BEGIN

class SceneManager {
private:
    struct SceneData{
        SceneData() : scene(-1), path() {}
        SceneData(ResourceHandle<Scene> scene, const std::string& path) : scene(scene), path(path) {}
        ResourceHandle<Scene> scene;
        std::string path;
    };
public:
    static SceneManager& instance();
    static void create();
    static void destroy();
    static Scene& load(const std::string& sceneName);
    static Scene& current_scene();
    virtual ~SceneManager() = default;

    void store(const std::string &path);
    bool is_active() { return m_currentScene.valid(); }

protected:

    static SceneManager* s_instance;

    SceneManager() : m_currentScene(-1) {}

    Scene& load_scene(const std::string& path);
    Scene& get_current_scene();

private:
    std::unordered_map<std::string, SceneData> m_scenes;
    ResourceHandle<Scene> m_currentScene;


};

EG_ENGINE_END


#endif //EAGLE_SCENEMANAGER_H
