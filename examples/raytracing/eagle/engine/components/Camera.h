//
// Created by Novak on 29/03/2020.
//

#ifndef EAGLE_CAMERA_H
#define EAGLE_CAMERA_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct Camera {
public:
    Camera() = default;
    Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up,
            float fov, float aspectRatio, float nearPlane = 0.1, float farPlane = 1000) :
            m_position(position), m_center(center), m_up(up),
            m_fov(fov), m_aspectRatio(aspectRatio), m_nearPlane(nearPlane), m_farPlane(farPlane){
        m_view = glm::lookAt(m_position, m_center, m_up);
        m_projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    inline void set_camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up,
                           float fov, float aspectRatio, float nearPlane = 0.1, float farPlane = 1000){
        m_position = position;
        m_center = center;
        m_up = up;
        m_fov = fov;
        m_aspectRatio = aspectRatio;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_view = glm::lookAt(m_position, m_center, m_up);
        m_projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }

    inline void set_position(const glm::vec3& position){
        m_position = position;
        m_view = glm::lookAt(m_position, m_center, m_up);
    }

    inline void set_view(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up){
        m_position = position;
        m_center = center;
        m_up = up;
        m_view = glm::lookAt(m_position, m_center, m_up);
    }

    inline void set_projection(float fov, float aspectRatio, float nearPlane = 0.1, float farPlane = 1000){
        m_fov = fov;
        m_aspectRatio = aspectRatio;
        m_nearPlane = nearPlane;
        m_farPlane = farPlane;
        m_projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
        m_projection[1][1] *= -1;
    }

    inline void set_aspect_ratio(float aspectRatio){
        m_aspectRatio = aspectRatio;
        m_projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
        m_projection[1][1] *= -1;
    }



    inline glm::vec3 position() const { return m_position; }
    inline glm::vec3 center() const { return m_center; }
    inline glm::vec3 up() const { return m_up; }
    inline glm::mat4 view_matrix() const { return m_view; }
    inline glm::mat4 projection_matrix() const { return m_projection; }
    inline glm::mat4 view_projection_matrix() const { return m_projection * m_view; }
    inline float aspect_ratio() const { return m_aspectRatio; }

private:
    glm::vec3 m_position, m_center, m_up;
    float m_fov, m_aspectRatio, m_nearPlane, m_farPlane;
    glm::mat4 m_view, m_projection;

};

struct OnCameraUpdate {
    OnCameraUpdate(const Camera& camera) : camera(camera) {}
    const Camera& camera;
};

EG_ENGINE_END

#endif //EAGLE_CAMERA_H
