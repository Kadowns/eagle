//
// Created by Novak on 11/04/2020.
//

#ifndef EAGLE_RAYTRACERDATA_H
#define EAGLE_RAYTRACERDATA_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct RaytracerData {
    struct{
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 inverseProjection;
        alignas(16) glm::vec3 position;
        alignas(8) glm::vec2 pixelOffset;
        alignas(16) glm::vec4 light;
        float sampleCount;
        int sphereCount;
        int boxCount;
    } ubo;

    struct SphereData {
        alignas(16) glm::vec3 position;
        alignas(4) float radius;
        alignas(16) glm::vec3 albedo;
        alignas(16) glm::vec3 specular;
    };

    struct BoxData {
        alignas(16) glm::vec3 center;
        alignas(16) glm::vec3 radius;
        alignas(16) glm::vec3 albedo;
        alignas(16) glm::vec3 specular;
        alignas(16) glm::mat4 rotation;
        alignas(16) glm::mat4 inverseRotation;
    };

    std::array<SphereData, MAX_SPHERES> spheresData;
    std::array<BoxData, MAX_SPHERES> boxesData;
    Handle<UniformBuffer> uniformBuffer;
    struct {
        Handle<Texture> color, skybox;
        Handle<ComputeShader> shader;
        Handle<StorageBuffer> spheresBuffer, boxesBuffer;
    } compute;

    struct{
        Handle<Shader> shader;
        Handle<DescriptorSet> descriptorSet;
    } quad;
};

EG_ENGINE_END

#endif //EAGLE_RAYTRACERDATA_H
