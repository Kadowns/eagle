#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec4 aColor;

layout(location = 0) out vec4 vColor;

layout(binding = 0, set = 0) uniform TransformUbo {
    mat2 model;
} uTransformUbo;

out gl_PerVertex{
    vec4 gl_Position;
};


void main() {
    vColor = aColor;
    gl_Position = vec4(uTransformUbo.model * aPosition, 0.0f, 1.0f);
}