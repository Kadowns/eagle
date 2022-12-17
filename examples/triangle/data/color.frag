#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform ColorUBO {
        vec4 color;
} uColor;

layout(location = 0) in vec4 vColor;

layout(location = 0) out vec4 outColor;

void main() {
        outColor = vColor * uColor.color;
//    outColor = vec4(pow(texture(uTexture, vTexCoord).rgb, vec3(1.0/2.2)), 1.0f);
//    outColor = vec4(texture(uTexture, vTexCoord).rgb, 1);
}