//
// Created by Novak on 15/09/2019.
//

#include <eagle/engine/resources/ShaderModule.h>
#include <eagle/engine/Renderer.h>

EG_ENGINE_BEGIN

ShaderModule::ShaderModule(const std::string &filepath, const std::string &name) : Resource(name), m_filepath(filepath) {

    std::string source;
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        source.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&source[0], source.size());
        file.close();
    }
    m_source = source;

    parse(*this);

    VertexLayout layout(8, {
            Format::R32G32B32_SFLOAT,
            Format::R32G32_SFLOAT,
            Format::R32G32B32_SFLOAT
    });

    ShaderPipelineInfo pipelineInfo(layout);
    pipelineInfo.offscreenRendering = true;
    pipelineInfo.depthTesting = true;
    pipelineInfo.blendEnable = false;
    pipelineInfo.dynamicStates = false;

    m_shader = Renderer::instance().context().create_shader(m_shaderStages, pipelineInfo);
}

void ShaderModule::parse(ShaderModule &module) {
    size_t pos = module.m_source.find_first_of('#', 0);
    while (pos != std::string::npos) {
        size_t eol = module.m_source.find_first_of("\r\n", pos);

        parse_line(module, module.m_source.substr(pos, eol - pos));

        size_t nextLinePos = module.m_source.find_first_not_of("\r\n", eol);
        pos = module.m_source.find('#', nextLinePos);
    }
}

void ShaderModule::parse_line(ShaderModule &module, const std::string &line) {

    size_t tokenBegin = line.find_first_of('#', 0);
    size_t tokenEnd = line.find_first_of(' ', tokenBegin);
    ShaderFileToken tokenType = get_token_type(line.substr(tokenBegin, tokenEnd - tokenBegin));

    size_t eol = line.length() - 1;
    switch(tokenType){
        case ShaderFileToken::STAGE:
            parse_shader_stage(module, line, tokenEnd, eol);
            break;
        case ShaderFileToken::INVALID:
            throw std::runtime_error("ShaderFile token syntax error! token was: " + line.substr(tokenBegin, tokenEnd - tokenBegin));
    }
}

void ShaderModule::parse_shader_stage(ShaderModule &module, const std::string &line,
                                      std::string::size_type pos, std::string::size_type eol) {

    size_t stageBegin = line.find_first_not_of(' ', pos);
    size_t stageEnd = line.find_first_of(' ', stageBegin);

    if (stageBegin > eol || stageBegin == std::string::npos || stageEnd > eol || stageEnd == std::string::npos){
        throw std::runtime_error("Invalid shader stage name!");
    }

    ShaderStage stage = shader_stage_from_string(line.substr(stageBegin, stageEnd - stageBegin));

    if (stage == ShaderStage::INVALID){
        throw std::runtime_error("Invalid shader stage name!");
    }

    size_t pathBegin = line.find_first_of('\"', stageEnd);
    size_t pathEnd = line.find_first_of('\"', pathBegin + 1) - 1;

    if (pathBegin > eol || pathBegin == std::string::npos || pathEnd > eol || pathEnd == std::string::npos){
        throw std::runtime_error("Invalid shader stage filepath!");
    }

    module.m_shaderStages[stage] = line.substr(pathBegin + 1, pathEnd - pathBegin);
}

EG_ENGINE_END
