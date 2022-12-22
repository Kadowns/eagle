//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include <eagle/renderer/renderer_global_definitions.h>
#include <eagle/renderer/vertex_layout.h>
#include <eagle/renderer/descriptor_set_layout.h>
#include <eagle/renderer/render_pass.h>

namespace eagle {

struct ShaderStageDescription {
    ShaderStage stage = ShaderStage::INVALID;
    std::vector<uint32_t> spirVCode;
    std::string entryPointName = "main";
};

struct ShaderCreateInfo {
    std::vector<ShaderStageDescription> shaderStages;
    RenderPass* renderPass = nullptr;
    VertexLayout vertexLayout;
    PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
    CullMode cullMode = CullMode::BACK_BIT;
    struct{
        float x = 0, y = 0;
        float widthPercent = 1, heightPercent = 1;
    } viewport;
    bool blendEnable = false;
    bool depthTesting = false;
    bool dynamicStates = false;
};

class Shader {
public:
    explicit Shader(ShaderCreateInfo createInfo) : m_createInfo(std::move(createInfo)) {}

    virtual DescriptorSetLayout* descriptor_set_layout(uint32_t index) const = 0;

    virtual size_t descriptor_set_layout_count() const = 0;

    static std::vector<uint32_t> read_spir_v_code(const std::string& filename);

protected:
    ShaderCreateInfo m_createInfo;
};

}

#endif //EAGLE_SHADER_H
