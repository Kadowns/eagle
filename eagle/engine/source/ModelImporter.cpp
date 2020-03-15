//
// Created by Novak on 19/02/2020.
//

#include <eagle/engine/ModelImporter.h>
#include <eagle/engine/geometry/Vertex.h>

EG_ENGINE_BEGIN

std::pair<Mesh*, Material*> ModelImporter::load_mesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
//    std::vector<Texture> textures;

    for(uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex = {};
        // process vertex positions, normals and texture coordinates
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // does the mesh contain texture coordinates?
        if(mesh->mTextureCoords[0]){
            vertex.uv.x = mesh->mTextureCoords[0][i].x;
            vertex.uv.y = mesh->mTextureCoords[0][i].y;
        }
        else{
            vertex.uv = glm::vec2(0);
        }

        vertices.push_back(vertex);
    }

    for(uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(uint32_t j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    Material* mat = nullptr;
    // process material
    if(mesh->mMaterialIndex >= 0) {
        mat = load_material(scene->mMaterials[mesh->mMaterialIndex]);
        ResourceID id = ResourcesPool::instance().count<Material>();
        ResourcesPool::instance().store<Material>(mat, id);
    }

    VertexLayout layout(8, {
            Format::R32G32B32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R32G32B32_SFLOAT,
    });

    Mesh* m = new Mesh(layout, vertices.data(), indices.data(), vertices.size(), indices.size(), mesh->mName.C_Str());
    ResourceID id = ResourcesPool::instance().count<Mesh>();
    ResourcesPool::instance().store<Mesh>(m, id);

    return std::pair<Mesh*, Material*>(m, mat);
}

void ModelImporter::process_node(aiNode *node, const aiScene *scene) {

    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.emplace_back(load_mesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

Material *ModelImporter::load_material(aiMaterial *aimat) {
    Material* mat = new Material(ResourceHandle<ShaderModule>(0), aimat->GetName().C_Str());
    std::vector<Texture*> diffuseMaps = load_material_textures(aimat, aiTextureType_DIFFUSE);
    for (int i = 0; i < diffuseMaps.size(); ++i) {
        ResourceID id = ResourcesPool::instance().count<Texture>() + i;
        ResourcesPool::instance().store<Texture>(diffuseMaps[i], id);
        mat->set_texture("uDiffuse" + std::to_string(i), id);
    }

    std::vector<Texture*> specularMaps = load_material_textures(aimat, aiTextureType_SPECULAR);
    for (int i = 0; i < specularMaps.size(); ++i) {
        ResourceID id = ResourcesPool::instance().count<Texture>() + i;
        ResourcesPool::instance().store<Texture>(specularMaps[i], id);
        mat->set_texture("uSpecular" + std::to_string(i), id);
    }
    return mat;
}

ModelImporter::ModelImporter(const std::string &path) {

    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        EG_ERROR_F("ERROR::ASSIMP::{0}", import.GetErrorString());
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);

}

Model *ModelImporter::create_model() {
    return nullptr;
}

std::vector<Texture*> ModelImporter::load_material_textures(aiMaterial *mat, aiTextureType type) {

    std::vector<Texture*> textures;
    for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string fullpath = str.C_Str();
        std::string filename = fullpath.substr(fullpath.find_last_of("/\\"));
        Texture* texture = new Texture(TextureImporter::load_texture(m_directory + '/' + filename), str.C_Str());
        textures.push_back(texture);
    }
    return textures;
}


EG_ENGINE_END