//
// Created by Novak on 15/09/2019.
//

#ifndef EAGLE_SHADERMODULE_H
#define EAGLE_SHADERMODULE_H

#include <eagle/engine/GlobalDefs.h>
#include "ResourcesPool.h"

EG_ENGINE_BEGIN

class ShaderModule : public Resource<ShaderModule>{
private:

    enum class ShaderFileToken{
        INVALID = 0,
        STAGE = 2
    };

public:

    ShaderModule(const std::string &filepath, const std::string &name);
    inline const std::unordered_map<ShaderStage, std::string>& paths() const { return m_shaderStages; }
    inline Reference<Shader> shader() { return m_shader.lock(); }

private:

    static void parse(ShaderModule& module);
    static void parse_line(ShaderModule &module, const std::string &line);
    static void parse_shader_stage(ShaderModule &module, const std::string &line, std::string::size_type pos,
                            std::string::size_type eol);


    //helper functions
    inline static ShaderFileToken get_token_type(const std::string& token){
        if (token == "#stage"){
            return ShaderFileToken::STAGE;
        }
        return ShaderFileToken::INVALID;
    }

    inline static ShaderStage shader_stage_from_string(const std::string& stage){
        if (stage == "vert"){
            return ShaderStage::VERTEX;
        }
        if (stage == "frag"){
            return ShaderStage::FRAGMENT;
        }
        return ShaderStage::INVALID;
    }

private:

    std::string m_filepath;
    std::string m_source;
    std::unordered_map<ShaderStage, std::string> m_shaderStages;
    Handle<Shader> m_shader;
};

EG_ENGINE_END

#endif //EAGLE_SHADERMODULE_H
