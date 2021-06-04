//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include "renderer_global_definitions.h"
#include "vertex_layout.h"
#include "descriptor_set_layout.h"
#include "render_pass.h"

namespace eagle {


struct ShaderCreateInfo {
    ShaderCreateInfo(const WeakPointer<RenderPass>& renderPass,
                     const std::unordered_map<ShaderStage, std::string>& shaderStages) :
                     renderPass(renderPass),
                     shaderStages(shaderStages){}
    WeakPointer<RenderPass> renderPass;
    std::unordered_map<ShaderStage, std::string> shaderStages;
    bool blendEnable = false;
    bool depthTesting = false;
    bool dynamicStates = false;
    VertexLayout vertexLayout;
    PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
    CullMode cullMode = CullMode::BACK_BIT;
    struct{
        float x = 0, y = 0;
        float widthPercent = 1, heightPercent = 1;
    } viewport;
};

class Shader {

public:
    explicit Shader(const ShaderCreateInfo& createInfo) : m_createInfo(createInfo) {}
    virtual ~Shader() = default;

    virtual const std::vector<WeakPointer<DescriptorSetLayout>> get_descriptor_set_layouts() = 0;
    virtual const WeakPointer<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) = 0;
protected:
    ShaderCreateInfo m_createInfo;

};

}

#endif //EAGLE_SHADER_H
