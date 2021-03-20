//
// Created by Ricardo on 2/20/2021.
//

#ifndef EG_DESKTOPFILESYSTEM_H
#define EG_DESKTOPFILESYSTEM_H

#include <eagle/file_system.h>

namespace eagle {

class DesktopFileSystem : public FileSystem {
public:
    static void init();
    std::vector<uint8_t> read_bytes(const std::string &path) override;
    std::string read_text(const std::string& path) override;
};

}


#endif //EG_DESKTOPFILESYSTEM_H
