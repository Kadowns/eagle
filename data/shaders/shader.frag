#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "light.glsl"

layout(binding = 1) uniform Color {
    vec4 color;
} uColor;

layout(binding = 2) uniform sampler2D uTexture;

layout(binding = 3) uniform Lights {
    DirectionalLight directional;
} uLight;

layout(location = 0) in vec2 vTexCoord;
layout(location = 1) in vec3 vNormal;

layout(location = 0) out vec4 outColor;

void main() {

    vec3 diffuse = vec3(0.0);

    calculate_directional_light(uLight.directional, vNormal, diffuse);

    outColor = uColor.color * texture(uTexture, vTexCoord) * vec4(diffuse, 1.0);
}
