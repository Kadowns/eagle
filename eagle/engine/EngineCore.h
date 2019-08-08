//
// Created by Novak on 26/06/2019.
//

#ifndef EAGLE_ENGINECORE_H
#define EAGLE_ENGINECORE_H

#include <eagle/Eagle.h>
#include <entityx/entityx.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define EG_ENGINE_BEGIN namespace Eagle::Engine{
#define EG_ENGINE_END }

EG_ENGINE_BEGIN

const std::string ProjectRoot = PROJECT_ROOT;


Texture2DCreateInfo load_texture(const std::string& path);

Texture2DCreateInfo load_font(const std::string& path);

EG_ENGINE_END

#endif //EAGLE_ENGINECORE_H
