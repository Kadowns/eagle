//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include "RenderingCore.h"
#include "VertexLayout.h"
#include "DescriptorSetLayout.h"
#include "RenderPass.h"

EG_BEGIN


struct ShaderPipelineInfo {
    ShaderPipelineInfo(const Reference<RenderPass>& renderPass) : renderPass(renderPass) {}
    Reference<RenderPass> renderPass;
    bool blendEnable;
    bool depthTesting;
    bool dynamicStates;
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

    virtual const std::vector<Handle<DescriptorSetLayout>> get_descriptor_set_layouts() = 0;
    virtual const Handle<DescriptorSetLayout> get_descriptor_set_layout(uint32_t index) = 0;


};

EG_END

#endif //EAGLE_SHADER_H
