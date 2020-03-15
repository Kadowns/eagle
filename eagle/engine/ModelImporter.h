//
// Created by Novak on 19/02/2020.
//

#ifndef EAGLE_MODELIMPORTER_H
#define EAGLE_MODELIMPORTER_H

#include "GlobalDefs.h"
#include <eagle/engine/resources/Mesh.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <eagle/engine/resources/Material.h>
#include <eagle/engine/resources/Model.h>

EG_ENGINE_BEGIN

class ModelImporter {

public:

    ModelImporter(const std::string& path);
    Model* create_model();

private:
    void process_node(aiNode *node, const aiScene *scene);
    std::pair<Mesh *, Material *> load_mesh(aiMesh *mesh, const aiScene *scene);
    Material *load_material(aiMaterial *mat);
    std::vector<Texture *> load_material_textures(aiMaterial *mat, aiTextureType type);

private:
    std::vector<std::pair<Mesh*, Material*>> m_meshes;
    std::string m_directory;

};

EG_ENGINE_END

#endif //EAGLE_MODELIMPORTER_H
