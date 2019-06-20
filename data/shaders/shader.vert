#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "mvp.glsl"

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec2 vTexCoord;
layout(location = 1) out vec3 vNormal;

void main() {
    gl_Position = mvp.matrix * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    vNormal = mat3(mvp.matrix) * aNormal;
}
