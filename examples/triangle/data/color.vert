#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

layout(location = 0) out vec3 vColor;

out gl_PerVertex{
    vec4 gl_Position;
};

void main() {
    vColor = aColor;
    gl_Position = vec4(aPosition, 0.0f, 1.0f);
}