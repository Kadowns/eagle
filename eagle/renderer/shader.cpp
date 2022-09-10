//
// Created by Ricardo on 10/09/2022.
//

#include <eagle/renderer/shader.h>
#include <eagle/file_system.h>

namespace eagle {

std::vector<uint32_t> Shader::read_spir_v_code(const std::string& filename) {
    auto bytes = FileSystem::instance()->read_bytes(filename);
    std::vector<uint32_t> code(bytes.size() / sizeof(uint32_t));
    std::memcpy(code.data(), bytes.data(), bytes.size());
    return code;
}

}