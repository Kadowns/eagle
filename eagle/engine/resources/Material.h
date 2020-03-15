//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_MATERIAL_H
#define EAGLE_MATERIAL_H

#include <eagle/engine/Camera.h>
#include <eagle/engine/GlobalDefs.h>
#include "ResourcesPool.h"
#include "Texture.h"
#include "ShaderModule.h"

EG_ENGINE_BEGIN

class Material : public Resource<Material> {
private:

    enum class MaterialFileToken{
        INVALID = 0,
        SHADER = 1,
        TEXTURE = 2
    };


public:

    Material(const ResourceHandle <ShaderModule> &shader, const std::string &name);
    ~Material();

    ResourceHandle<ShaderModule> shader_module() { return m_shaderModule; }

    const std::map<std::string, DescriptorBindingDescription>& bindings() const { return m_bindingMap; }

    void set_uniform_bytes(const std::string &bindingName, const std::string &memberName, void *bytes);
    void set_texture(const std::string& name, ResourceHandle<Texture> texture);

    template<typename T>
    void set_uniform(const std::string& bindingName, const std::string& memberName, T value){
        set_uniform_bytes(bindingName, memberName, &value);
    }

    template<typename T>
    T get_uniform(const std::string& bindingName, const std::string& memberName){
        auto buffer = m_buffers.find(bindingName);
        if (buffer == m_buffers.end()){
            EG_ERROR_F("Invalid name for binding name: {0}!", bindingName);
            return T();
        }

        auto& description = m_bindingMap[bindingName];

        auto member = description.members.find(memberName);
        if (member == description.members.end()){
            EG_ERROR_F("Invalid member name on binding {0}: {1}!", bindingName, memberName);
            return T();
        }
        T value;
        memcpy(&value, buffer->second.lock()->bytes().data() + member->second.offset, member->second.size);
        return value;
    }

    void update();
    void bind(Scope <CommandBuffer> &commandBuffer, const Reference <DescriptorSet> &camera,
              const Reference <DescriptorSet> &world);


    ResourceHandle<Texture> get_texture(const std::string& name);
    const std::map<std::string, ResourceHandle<Texture>>& get_textures() const;

private:

    std::vector<Reference<DescriptorItem>> get_descriptor_items();

    ResourceHandle<ShaderModule> m_shaderModule;
    Handle<DescriptorSet> m_descriptor;
    std::map<std::string, DescriptorBindingDescription> m_bindingMap;
    std::map<std::string, ResourceHandle<Texture>> m_textures;
    std::map<std::string, Handle<UniformBuffer>> m_buffers;

};

EG_ENGINE_END


#endif //EAGLE_MATERIAL_H
