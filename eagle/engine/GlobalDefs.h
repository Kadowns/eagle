//
// Created by Novak on 26/06/2019.
//

#ifndef EAGLE_GLOBALDEFS_H
#define EAGLE_GLOBALDEFS_H

#include <eagle/core/Core.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/type_index.hpp>


#include <nlohmann/json.hpp>
// for convenience
using json = nlohmann::json;

#define EG_ENGINE_BEGIN namespace Eagle::Engine{
#define EG_ENGINE_END }

EG_ENGINE_BEGIN

const std::string ProjectRoot = PROJECT_ROOT;


//Texture2DCreateInfo load_texture(const std::string& path);
//
//Texture2DCreateInfo load_font(const std::string& path);

EG_ENGINE_END

#endif //EAGLE_GLOBALDEFS_H
