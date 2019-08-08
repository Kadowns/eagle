//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFER_H
#define EAGLE_UNIFORMBUFFER_H

#include <memory>

#include "DescriptorItem.h"
#include "Shader.h"

_EAGLE_BEGIN

class UniformBuffer : public DescriptorItem {

public:

    explicit UniformBuffer(size_t size);
    virtual ~UniformBuffer();

    virtual void flush(uint32_t bufferIndex) = 0;
    size_t size() {return m_size;}
protected:
    size_t m_size;
};

_EAGLE_END
#endif //EAGLE_UNIFORMBUFFER_H
