//
// Created by Novak on 01/11/2019.
//

#ifndef EAGLE_RENDERER_H
#define EAGLE_RENDERER_H

#include "GlobalDefs.h"

EG_ENGINE_BEGIN

class Renderer {
public:

    static void initialize(Window &window);
    static void terminate();

    static Renderer& instance();

    inline RenderingContext& context() { return *m_renderingContext; }

private:

    Renderer(Window &window);
    ~Renderer();

    static Renderer* s_instance;

    Reference<RenderingContext> m_renderingContext;
};

EG_ENGINE_END

#endif //EAGLE_RENDERER_H
