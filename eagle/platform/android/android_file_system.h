//
// Created by Ricardo on 2/20/2021.
//

#ifndef EG_ANDROIDFILESYSTEM_H
#define EG_ANDROIDFILESYSTEM_H

#include <eagle/file_system.h>

struct AAssetManager;

namespace eagle {

class AndroidFileSystem : public FileSystem {
public:
    static void init(AAssetManager* assetManager);

    std::vector<uint8_t> read_bytes(const std::string &path) override;
    std::string read_text(const std::string &path) override;
private:
    AndroidFileSystem(AAssetManager* assetManager);
    AAssetManager* m_assetManager;
};

}

#endif //EG_ANDROIDFILESYSTEM_H
