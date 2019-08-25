//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_MATERIAL_H
#define EAGLE_MATERIAL_H

#include "../EngineCore.h"
#include "ResourcesPool.h"
#include <eagle/engine/Camera.h>

EG_ENGINE_BEGIN

class Material : public Resource<Material> {
public:
    using MaterialItem = std::pair<std::string, Handle<DescriptorItem>>;
public:

    Material(Handle <Shader> shader, Reference <DescriptorSetLayout> descriptorSetLayout,
             std::vector<MaterialItem> items, const std::string &name = "Material");
    ~Material();

    Reference<Shader> shader() { return m_shader.lock(); }

    void set_uniform(const std::string& name, void* data);
    void bind(const Reference <Camera> &camera);

private:

    Handle<Shader> m_shader;
    Handle<DescriptorSet> m_descriptor;
    std::map<std::string, Handle<DescriptorItem>> m_itemMap;
    Reference<RenderingContext> m_renderingContext;

};

EG_ENGINE_END


#endif //EAGLE_MATERIAL_H
