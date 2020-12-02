//
// Created by Novak on 17/03/2020.
//

#ifndef EAGLE_ENGINEGLOBALDEFINITIONS_H
#define EAGLE_ENGINEGLOBALDEFINITIONS_H

#include <eagle/core/Core.h>

#include <entityx/entityx.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/norm.hpp>

#define MAX_SPHERES 256
#define MAX_BOXES 32

#define EG_ENGINE_BEGIN namespace Eagle::Engine {
#define EG_ENGINE_END   }

inline const std::string ProjectRoot = "data";

#endif //EAGLE_COREGLOBALDEFINITIONS_H