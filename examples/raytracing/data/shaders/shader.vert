#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform VP {
    mat4 mat;
} uWorld;

layout(push_constant) uniform Model{
    mat4 mat;
} uModel;


layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 vTexCoord;

void main() {
    gl_Position = uWorld.mat * uModel.mat * vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
}
