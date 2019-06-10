//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include <vector>
#include <map>

#include "eagle/core/Core.h"
#include "ShaderItemLayout.h"

_EAGLE_BEGIN

class Shader {

public:
    Shader(const std::string& vertFileName, const std::string& fragFileName);
    virtual ~Shader() = default;

    virtual void bind() = 0;
    virtual void compile() = 0;
    virtual void create_pipeline() = 0;
    virtual void cleanup_pipeline() = 0;
    virtual ShaderItemLayout get_shader_item(const std::string& name);

protected:

    static std::vector<char> load_shader(const std::string& fileName);

    std::vector<char> m_vertShaderCode, m_fragShaderCode;
    std::map<std::string, ShaderItemLayout> m_uniformLayouts;

};

_EAGLE_END

#endif //EAGLE_SHADER_H
