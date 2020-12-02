#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (push_constant) uniform PushConstants {
    vec2 scale;
    vec2 translate;
} pushConstants;

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec4 vColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    gl_Position = vec4(aPosition * pushConstants.scale + pushConstants.translate, 0.0, 1.0);
    vTexCoord = aTexCoord;
    vColor = aColor;
}