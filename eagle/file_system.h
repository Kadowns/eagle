//
// Created by Ricardo on 2/20/2021.
//

#ifndef EG_FILESYSTEM_H
#define EG_FILESYSTEM_H

#include <eagle/core_global_definitions.h>

namespace eagle {

class FileSystem {
public:
    static inline FileSystem* instance() { return s_instance; }
    virtual std::vector<uint8_t> read_bytes(const std::string& path) = 0;
    virtual std::string read_text(const std::string& path) = 0;

protected:
    static FileSystem* s_instance;

};

}

#endif //EG_FILESYSTEM_H
