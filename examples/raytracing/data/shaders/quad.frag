#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D uTexture;

layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
//    outColor = vec4(vTexCoord, 0.0f, 1.0f);
    outColor = vec4(pow(texture(uTexture, vTexCoord).rgb, vec3(1.0/2.2)), 1.0f);
    //outColor = vec4(texture(uTexture, vTexCoord).rgb, 1);
}