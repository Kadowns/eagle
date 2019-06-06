//
// Created by Novak on 06/06/2019.
//

#include <memory>
#include "eagle/renderer/RenderingContext.h"


_EAGLE_BEGIN

std::unique_ptr<RenderingContext> RenderingContext::m_currentRenderer;

std::shared_ptr<Shader>
RenderingContext::create_shader(const std::string &vertFilePath, const std::string &fragFilePath) {
    return m_currentRenderer->handle_create_shader(vertFilePath, fragFilePath);
}


_EAGLE_END


