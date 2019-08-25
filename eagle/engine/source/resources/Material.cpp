//
// Created by Novak on 30/07/2019.
//


#include "eagle/engine/resources/Material.h"

EG_ENGINE_BEGIN

Material::Material(Handle <Shader> shader, Reference <DescriptorSetLayout> descriptorSetLayout,
                   std::vector<MaterialItem> items, const std::string &name) : Resource(name),
    m_shader(shader), m_renderingContext(Application::instance().get_window()->get_rendering_context()) {

    std::vector<Reference<DescriptorItem>> descriptorItems;
    for (const MaterialItem& item : items){
        m_itemMap.insert(item);
        descriptorItems.emplace_back(item.second.lock());
    }

    m_descriptor = m_renderingContext->create_descriptor_set(descriptorSetLayout, descriptorItems);
}

Material::~Material() {

}

void Material::set_uniform(const std::string &name, void *data) {

    auto buffer = m_itemMap.find(name);
    if (buffer == m_itemMap.end()){
        EG_ERROR_F("Invalid key for uniform: {0}!", name);
    }
    auto uniformBuffer = std::static_pointer_cast<UniformBuffer>(buffer->second.lock());

    m_renderingContext->uniform_buffer_flush(uniformBuffer, data);
}

void Material::bind(const Reference <Camera> &camera) {
    m_renderingContext->bind_shader(m_shader.lock());
    m_renderingContext->bind_descriptor_sets(m_shader.lock(), camera->descriptor(), 0);
    m_renderingContext->bind_descriptor_sets(m_shader.lock(), m_descriptor.lock(), 1);
}


EG_ENGINE_END