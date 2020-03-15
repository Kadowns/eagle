//
// Created by Novak on 30/07/2019.
//


#include "eagle/engine/resources/Material.h"
#include "eagle/engine/Renderer.h"


EG_ENGINE_BEGIN

Material::Material(const ResourceHandle <ShaderModule> &shader, const std::string &name) :
    Resource(name),
    m_shaderModule(shader){

    auto layout = m_shaderModule->shader()->get_descriptor_set_layout(1).lock();
    
    RenderingContext& context = Renderer::instance().context();

    for (const DescriptorBindingDescription& item : layout->bindings()){
        m_bindingMap.emplace(item.name, item);
        if (item.descriptorType == DescriptorType::UNIFORM_BUFFER){
            m_buffers[item.name] = context.create_uniform_buffer(item.size, nullptr);
        }else{
            m_textures[item.name] = ResourceHandle<Texture>();
        }
    }

    m_descriptor = context.create_descriptor_set(layout, get_descriptor_items());
}

Material::~Material() {

}

void Material::set_uniform_bytes(const std::string &bindingName, const std::string &memberName, void *bytes) {

    auto buffer = m_buffers.find(bindingName);
    if (buffer == m_buffers.end()){
        EG_ERROR_F("Invalid name for binding name: {0}!", bindingName);
        return;
    }

    auto& description = m_bindingMap[bindingName];

    auto member = description.members.find(memberName);
    if (member == description.members.end()){
        EG_ERROR_F("Invalid member name on binding {0}: {1}!", bindingName, memberName);
        return;
    }
    buffer->second.lock()->set_bytes(bytes, member->second.size, member->second.offset);
    buffer->second.lock()->update();
}



void Material::bind(Scope <CommandBuffer> &commandBuffer,
                    const Reference <DescriptorSet> &camera,
                    const Reference <DescriptorSet> &world) {
    commandBuffer->bind_shader(m_shaderModule->shader());
    commandBuffer->bind_descriptor_sets(m_shaderModule->shader(), camera, 0);
    commandBuffer->bind_descriptor_sets(m_shaderModule->shader(), m_descriptor.lock(), 1);
    commandBuffer->bind_descriptor_sets(m_shaderModule->shader(), world, 2);
}

void Material::set_texture(const std::string &name, ResourceHandle<Texture> texture) {
    if (m_bindingMap.find(name) == m_bindingMap.end()){
        EG_ERROR_F("Invalid name for texture: {0}!", name);
        return;
    }
    m_textures[name] = texture;
}

std::vector<Reference<DescriptorItem>> Material::get_descriptor_items() {

    std::vector<Reference<DescriptorItem>> descriptorItems;
    for (auto& binding : m_bindingMap){
        if (binding.second.descriptorType == DescriptorType::UNIFORM_BUFFER){
            descriptorItems.emplace_back(m_buffers[binding.first].lock());
        }else{
            descriptorItems.emplace_back(m_textures[binding.first]->handle().lock()->get_image());
        }
    }
    return descriptorItems;
}

void Material::update() {
    m_descriptor.lock()->update(get_descriptor_items());
}

ResourceHandle<Texture> Material::get_texture(const std::string &name) {
    if (m_textures.find(name) == m_textures.end()){
        throw std::runtime_error("Invalid texture name!");
    }
    return m_textures[name];
}

const std::map<std::string, ResourceHandle<Texture>> &Material::get_textures() const {
    return m_textures;
}


EG_ENGINE_END