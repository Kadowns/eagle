//
// Created by Novak on 16/06/2019.
//

#include "eagle/core/renderer/vulkan/VulkanShaderCompiler.h"
#include "eagle/core/Log.h"

#include <iostream>
#include <fstream>

EG_BEGIN

bool VulkanShaderCompiler::m_glslangIntitialized = false;

std::vector<uint32_t> VulkanShaderCompiler::compile_glsl(const std::string &filename, EG_SHADER_STAGE stage) {

    //initializes glslang
    if (!m_glslangIntitialized) {
        if (!glslang::InitializeProcess()){
            EG_CORE_FATAL("Failed to initialize vulkan shader compiler!");
            throw std::runtime_error("Failed to initialize vulkan shader compiler!");
        }
        m_glslangIntitialized = true;
    }

    EShLanguage shaderStage = get_shader_stage(stage);

    //creates shader object based on shader stage
    glslang::TShader shader(shaderStage);


    std::ifstream file(filename);
    if (!file.is_open()) {
        EG_CORE_FATAL_F("Failed to open shader file: {0}", filename);
        throw std::runtime_error("failed to open file: " + filename);
    }

    //reads glsl file and stores on a const char* (otherwise it would not be possible to set the shader strings with a temporary value)
    std::string glslInput((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    const char* cstrInput = glslInput.data();

    shader.setStrings(&cstrInput, 1);

    //environment definitions
    int ClientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
    glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_0;
    glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

    shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
    shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);

    TBuiltInResource resources = DefaultTBuiltInResource;
    EShMessages messages = EShMsgDefault;

    const int DefaultVersion = 100;


    //preprocessing of the glsl shader (includes all get_paths into the actual glsl string)
    DirStackFileIncluder includer;

    std::string path = get_file_path(filename);
    includer.pushExternalLocalDirectory(path);

    std::string preprocessedGLSL;

    if (!shader.preprocess(&resources, DefaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, includer)) {
        EG_CORE_FATAL_F("GLSL Preprocessing Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    //updates shader strings with preprocessed glsl file
    const char* preprocessedCStr = preprocessedGLSL.c_str();
    shader.setStrings(&preprocessedCStr, 1);

    //parses the shader
    if (!shader.parse(&resources, 100, false, messages)) {
        EG_CORE_FATAL_F("GLSL Parsing Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    glslang::TProgram program;
    program.addShader(&shader);

    //links program with shader
    if(!program.link(messages)) {
        EG_CORE_FATAL_F("GLSL Linking Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    //converts glslang program to spirv format
    std::vector<uint32_t> spirv;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirv, &logger, &spvOptions);
    if (!logger.getAllMessages().empty()){
        EG_CORE_FATAL_F("Failed to convert glsl code to spirv code! Log: {0}", logger.getAllMessages());
    }
    return spirv;
}

std::string VulkanShaderCompiler::get_file_path(const std::string &file) {
    size_t found = file.find_last_of("/\\");
    return file.substr(0,found);
}

std::string VulkanShaderCompiler::get_suffix(const std::string &name) {
    const size_t pos = name.rfind('.');
    return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

EShLanguage VulkanShaderCompiler::get_shader_stage(EG_SHADER_STAGE stage) {
    switch(stage){
        case EG_SHADER_STAGE::VERTEX:return EShLangVertex;
        case EG_SHADER_STAGE::FRAGMENT:return EShLangFragment;
        case EG_SHADER_STAGE::COMPUTE:return EShLangCompute;
        case EG_SHADER_STAGE::GEOMETRY:return EShLangGeometry;
        case EG_SHADER_STAGE::TESSALATION_CONTROL:return EShLangTessControl;
        case EG_SHADER_STAGE::TESSALATION_EVALUATE:return EShLangTessEvaluation;
    }
}

EG_END
