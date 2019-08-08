//
// Created by Novak on 06/06/2019.
//

#ifndef EAGLE_SHADER_H
#define EAGLE_SHADER_H

#include <vector>
#include <map>

#include "RenderingCore.h"
#include "VertexLayout.h"
#include "DescriptorSetLayout.h"

_EAGLE_BEGIN


struct ShaderPipelineInfo {
    ShaderPipelineInfo(const VertexLayout& layout) : vertexLayout(layout) {}

    bool blendEnable;
    bool depthTesting;
    bool dynamicStates;
    VertexLayout vertexLayout;
    struct{
        float x = 0, y = 0;
        float widthPercent = 1, heightPercent = 1;
    } viewport;
};


class Shader {

public:
    Shader();
    virtual ~Shader() = default;

    virtual void create_pipeline() = 0;
    virtual void cleanup_pipeline() = 0;

};

_EAGLE_END

#endif //EAGLE_SHADER_H
