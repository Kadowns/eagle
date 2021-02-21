//
// Created by Ricardo on 2/20/2021.
//

#include "AndroidFileSystem.h"
#include <android/asset_manager.h>

void Eagle::AndroidFileSystem::init(AAssetManager *assetManager) {
    s_instance = new AndroidFileSystem(assetManager);
}

Eagle::AndroidFileSystem::AndroidFileSystem(AAssetManager *assetManager) : m_assetManager(assetManager){

}

std::vector<uint8_t> Eagle::AndroidFileSystem::read_bytes(const std::string &path) {
    AAsset* asset = AAssetManager_open(m_assetManager, path.c_str(), AASSET_MODE_STREAMING);
    if (!asset){
        throw std::runtime_error("failed to open file: " + path);
    }
    std::vector<uint8_t> bytes(AAsset_getLength(asset));
    AAsset_read(asset, bytes.data(), bytes.size());
    AAsset_close(asset);
    return std::move(bytes);
}

std::string Eagle::AndroidFileSystem::read_text(const std::string &path) {
    std::vector<uint8_t> bytes = std::move(read_bytes(path));
    return std::string(bytes.begin(), bytes.end());
}

