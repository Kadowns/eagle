//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFER_H
#define EAGLE_UNIFORMBUFFER_H

#include <memory>

#include "Shader.h"

_EAGLE_BEGIN

class UniformBuffer {

public:

    explicit UniformBuffer(const ShaderItemLayout& layout );
    virtual ~UniformBuffer();

    virtual void flush(void *data, uint32_t bufferIndex) = 0;
    virtual size_t size() = 0;
protected:
    ShaderItemLayout m_layout;

};

_EAGLE_END
#endif //EAGLE_UNIFORMBUFFER_H
