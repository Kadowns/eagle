//
// Created by Novak on 06/06/2019.
//

#include <fstream>

#include "eagle/renderer/Shader.h"
#include "eagle/core/Log.h"

_EAGLE_BEGIN


std::vector<char> Shader::load_shader(const std::string &fileName) {
    auto path = std::string(PROJECT_ROOT + fileName);
    EG_CORE_DEBUG_F("Shader file path: {0}", path);

    std::ifstream file(PROJECT_ROOT + fileName, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

Shader::Shader(const std::string &vertFileName, const std::string &fragFileName):
    m_vertShaderCode(load_shader(vertFileName)),
    m_fragShaderCode(load_shader(fragFileName)){
}

_EAGLE_END


