//
// Created by Novak on 08/05/2019.
//

#ifndef EAGLE_RENDERINGCONTEXT_H
#define EAGLE_RENDERINGCONTEXT_H

#include "eagle/core/Core.h"
#include "Shader.h"

_EAGLE_BEGIN

class Window;

class RenderingContext {
public:

    RenderingContext() { m_currentRenderer = std::unique_ptr<RenderingContext>(this); }
    virtual ~RenderingContext() = default;

    virtual void init(Window *window) = 0;
    virtual void refresh() = 0;
    virtual void deinit() = 0;

    static std::shared_ptr<Shader> create_shader(const std::string &vertFilePath, const std::string &fragFilePath);

protected:

    static std::unique_ptr<RenderingContext> m_currentRenderer;

    virtual std::shared_ptr<Shader> handle_create_shader(const std::string &vertFilePath,
                                                         const std::string &fragFilePath) = 0;

};

_EAGLE_END

#endif //EAGLE_RENDERINGCONTEXT_H
