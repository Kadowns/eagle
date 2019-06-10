//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFER_H
#define EAGLE_UNIFORMBUFFER_H

#include <memory>

#include "Shader.h"

_EAGLE_BEGIN

enum UNIFORM_BUFFER_USAGE {
    UNIFORM_BUFFER_USAGE_DATA   = BIT(1),
    UNIFORM_BUFFER_USAGE_IMAGE  = BIT(2)
};

class UniformBuffer {

public:

    explicit UniformBuffer(UNIFORM_BUFFER_USAGE usageFlags);
    virtual ~UniformBuffer();

    virtual void flush(void *data, uint32_t bufferIndex) = 0;

    inline UNIFORM_BUFFER_USAGE get_usage_flags() { return m_usageFlags;}

protected:
    UNIFORM_BUFFER_USAGE m_usageFlags;

};

_EAGLE_END
#endif //EAGLE_UNIFORMBUFFER_H
