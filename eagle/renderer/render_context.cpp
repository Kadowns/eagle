//
// Created by Ricardo on 23/08/2022.
//


#include <eagle/renderer/render_context.h>

namespace eagle {

RenderContext::RenderContext(const RenderContextCreateInfo& createInfo) :
    m_createInfo(createInfo) {

}

RenderContext::~RenderContext() = default;

}