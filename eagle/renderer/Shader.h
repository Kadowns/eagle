//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include "RenderingCore.h"
#include "VertexLayout.h"
#include "DescriptorSetLayout.h"
#include "RenderPass.h"

namespace eagle {


struct ShaderCreateInfo {
    ShaderCreateInfo(const std::shared_ptr<RenderPass>& renderPass,
                     const std::unordered_map<ShaderStage, std::string>& shaderStages) :
                     renderPass(renderPass),
                     shaderStages(shaderStages){}
    std::shared_ptr<RenderPass> renderPass;
    std::unordered_map<ShaderStage, std::string> shaderStages;
    bool blendEnable = false;
    bool depthTesting = false;
    bool dynamicStates = false;
    VertexLayout vertexLayout;
    PrimitiveTopology primitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
    struct{
        float x = 0, y = 0;
        float widthPercent = 1, heightPercent = 1;
    } viewport;
};

class Shader {

public:
    Shader() = default;
    virtual ~Shader() = default;

    virtual void create_pipeline() = 0;
    virtual void cleanup_pipeline() = 0;

    virtual const std::vector<std::weak_ptr<DescriptorSetLayout>> get_descriptor_set_layouts() = 0;
    virtual const std::weak_ptr<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) = 0;


};

}

#endif //EAGLE_SHADER_H
