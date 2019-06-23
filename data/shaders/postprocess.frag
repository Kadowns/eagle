#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform sampler2D uTexture;

layout(binding = 1) uniform Kernel{
    float amount;
    mat3 mat;
} uKernel;


layout(location = 0) in vec2 vTexCoord;

layout(location = 0) out vec4 outColor;

void main(){

    float dx = 1.0 / 1280;
    float dy = 1.0 / 720;

    vec3 color1 = uKernel.mat[0][0] * texture(uTexture, vTexCoord + vec2(-dx, -dy)).rgb;
    vec3 color2 = uKernel.mat[1][0] * texture(uTexture, vTexCoord + vec2(-dx, 0)).rgb;
    vec3 color3 = uKernel.mat[2][0] * texture(uTexture, vTexCoord + vec2(-dx, dy)).rgb;
    vec3 color4 = uKernel.mat[0][1] * texture(uTexture, vTexCoord + vec2(0, -dy)).rgb;
    vec3 color5 = uKernel.mat[1][1] * texture(uTexture, vTexCoord).rgb;
    vec3 color6 = uKernel.mat[2][1] * texture(uTexture, vTexCoord + vec2(0, dy)).rgb;
    vec3 color7 = uKernel.mat[0][2] * texture(uTexture, vTexCoord + vec2(dx, -dy)).rgb;
    vec3 color8 = uKernel.mat[1][2] * texture(uTexture, vTexCoord + vec2(dx, 0)).rgb;
    vec3 color9 = uKernel.mat[2][2] * texture(uTexture, vTexCoord + vec2(dx, dy)).rgb;

    vec3 borderColor = (color1 + color2 + color3 + color4 + color5 + color6 + color7 + color8 + color9).rrr;
    vec3 color = texture(uTexture, vTexCoord).rgb;

    outColor = vec4(mix(color, borderColor, uKernel.amount), 1.0);


}