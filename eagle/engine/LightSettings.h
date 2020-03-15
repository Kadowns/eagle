//
// Created by Novak on 07/11/2019.
//

#ifndef EAGLE_LIGHTSETTINGS_H
#define EAGLE_LIGHTSETTINGS_H

#include "GlobalDefs.h"
#include "Renderer.h"

#include "eagle/engine/components/Lights.h"

EG_ENGINE_BEGIN

struct LightBuffer{
    DirectionalLight directionalLight;
    alignas(16) glm::vec3 ambient;
};

class LightSettings {
public:
    LightSettings() {
        s_instance = this;
        m_lightBufferData.ambient = glm::vec3(0.1f);

        RenderingContext& context = Renderer::instance().context();

        DescriptorBindingDescription descriptorBinding = {};
        descriptorBinding.descriptorType = DescriptorType::UNIFORM_BUFFER;
        descriptorBinding.shaderStage = ShaderStage::FRAGMENT;
        descriptorBinding.binding = 0;
        descriptorBinding.size = sizeof(LightBuffer);
        descriptorBinding.name = "uScene";

        m_descriptorLayout = context.create_descriptor_set_layout({descriptorBinding});

        m_buffer = context.create_uniform_buffer(sizeof(LightBuffer), &m_lightBufferData);

        m_descriptor = context.create_descriptor_set(m_descriptorLayout.lock(), {m_buffer.lock()});

    }

    ~LightSettings(){

    }

    void set_ambient_light(const glm::vec3& light){
        m_lightBufferData.ambient = light;
    }

    void set_directional_light(const DirectionalLight& light){
        m_lightBufferData.directionalLight = light;
    }

    void update(){
        m_buffer.lock()->set_bytes(&m_lightBufferData, sizeof(m_lightBufferData), 0);
        m_buffer.lock()->update();
    }

    static LightSettings& instance() { return *s_instance; }

    inline const Reference<UniformBuffer> buffer() { return m_buffer.lock(); }
    inline const Reference<DescriptorSet> descriptor() { return m_descriptor.lock(); }
    inline const Reference<DescriptorSetLayout> descriptor_layout() { return m_descriptorLayout.lock(); }
private:
    static LightSettings* s_instance;


private:
    LightBuffer m_lightBufferData;
    Handle<UniformBuffer> m_buffer;
    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
};

EG_ENGINE_END

#endif //EAGLE_LIGHTSETTINGS_H
