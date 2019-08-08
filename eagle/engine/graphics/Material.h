//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_MATERIAL_H
#define EAGLE_MATERIAL_H

#include "eagle/engine/EngineCore.h"

EG_ENGINE_BEGIN

class Material {
public:
    using MaterialItem = std::pair<std::string, std::weak_ptr<DescriptorItem>>;
public:

    Material(std::weak_ptr<Shader> shader, std::shared_ptr<DescriptorSetLayout> descriptorSetLayout,
             std::vector<MaterialItem> items);
    ~Material();

    std::shared_ptr<Shader> shader() { return m_shader.lock(); }

    void set_uniform(const std::string& name, void* data);
    void bind();

private:

    std::weak_ptr<Shader> m_shader;
    std::weak_ptr<DescriptorSet> m_descriptor;
    std::map<std::string, std::weak_ptr<DescriptorItem>> m_itemMap;
    std::shared_ptr<RenderingContext> m_renderingContext;

};

EG_ENGINE_END


#endif //EAGLE_MATERIAL_H
