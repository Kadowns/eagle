//
// Created by Novak on 29/07/2019.
//

#ifndef EAGLE_CAMERA_H
#define EAGLE_CAMERA_H

#include "eagle/engine/GlobalDefs.h"
#include "eagle/engine/Renderer.h"

EG_ENGINE_BEGIN

class Camera {

public:

    Camera(float fov, float width, float height, float nearPlane, float farPlane, glm::vec3 position, glm::vec3 target,
           glm::vec3 up) :
            m_fov(fov),
            m_width(width),
            m_height(height),
            m_near(nearPlane),
            m_far(farPlane),
            m_position(position),
            m_target(target),
            m_up(up){

        m_projection = glm::perspective(glm::radians(fov), width / height, nearPlane, farPlane);

        m_view = glm::lookAt(position, target, up);
        m_viewProjection = m_projection * m_view;

        RenderingContext& context = Renderer::instance().context();

        m_buffer = context.create_uniform_buffer(sizeof(glm::mat4), &m_viewProjection);

        DescriptorBindingDescription binding = {};
        binding.name = "uWorld";
        binding.descriptorType = DescriptorType::UNIFORM_BUFFER;
        binding.shaderStage = ShaderStage::VERTEX;
        binding.binding = 0;

        m_descriptorLayout = context.create_descriptor_set_layout({binding});
        m_descriptor = context.create_descriptor_set(m_descriptorLayout.lock(), {m_buffer.lock()});
        m_renderTarget = context.create_render_target({RENDER_TARGET_ATTACHMENT::COLOR});
    }

    ~Camera() {}

    inline void set_position(glm::vec3 position){
        set_view(position, m_target, m_up);
    }
    inline void set_target(glm::vec3 target){
        set_view(m_position, target, m_up);
    }
    inline void set_up(glm::vec3 up){
        set_view(m_position, m_target, up);
    }
    inline void set_width(float width){
        set_projection(m_fov, width, m_height, m_near, m_far);
    }
    inline void set_height(float height){
        set_projection(m_fov, m_width, height, m_near, m_far);
    }
    inline void set_size(float width, float height){
        set_projection(m_fov, width, height, m_near, m_far);
    }
    inline void set_near(float nearPlane){
        set_projection(m_fov, m_width, m_height, nearPlane, m_far);
    }
    inline void set_far(float farPlane){
        set_projection(m_fov, m_width, m_height, m_near, farPlane);
    }
    inline void set_planes(float nearPlane, float farPlane){
        set_projection(m_fov, m_width, m_height, nearPlane, farPlane);
    }
    inline void set_fov(float fov){
        set_projection(fov, m_width, m_height, m_near, m_far);
    }
    inline void set_view(glm::vec3 position, glm::vec3 target, glm::vec3 up){
        m_position = position;
        m_target = target;
        m_up = up;
        m_view = glm::lookAt(position, target, up);
        update_buffer();
    }
    inline void set_projection(float fov, float width, float height, float nearPlane, float farPlane){
        m_fov = fov;
        m_width = width;
        m_height = height;
        m_near = nearPlane;
        m_far = farPlane;
        m_projection = glm::perspective(glm::radians(fov), width / height, nearPlane, farPlane);

        update_buffer();
    }

    inline void update_buffer(){
        m_buffer.lock()->upload_data(&matrix());
    }

    inline glm::mat4& matrix() { return m_viewProjection = m_projection * m_view; }

    inline Reference<DescriptorSetLayout> descriptor_layout() {return m_descriptorLayout.lock();}

    inline Reference<DescriptorSet> descriptor() { return m_descriptor.lock(); }

    inline Reference<RenderTarget> render_target() { return m_renderTarget.lock(); }

    inline glm::vec3 get_position() { return m_position; }

    inline glm::vec3 get_target() {return m_target; }


private:
    float m_fov;
    float m_width, m_height;
    float m_near, m_far;
    glm::vec3 m_position, m_target, m_up;

    glm::mat4 m_view, m_projection, m_viewProjection;

    Handle<UniformBuffer> m_buffer;
    Handle<DescriptorSetLayout> m_descriptorLayout;
    Handle<DescriptorSet> m_descriptor;
    Handle<RenderTarget> m_renderTarget;

};

EG_ENGINE_END

#endif //EAGLE_CAMERA_H
