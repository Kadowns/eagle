
struct DirectionalLight{
    vec4 direction;
    vec4 color;
};


void calculate_directional_light(in DirectionalLight light, in vec3 normal, out vec3 diff) {

    //diffuse
    vec3 L = normalize(light.direction.xyz);
    vec3 N = normalize(normal);
    float diffuseIntensity = max(dot(N, -L), 0.0);
    vec3 diffuse = diffuseIntensity * light.color.xyz;
    //------

    diff = clamp(diffuse, 0.0, 1.0);
}