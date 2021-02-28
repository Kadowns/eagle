//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <vector>
#include "eagle/CoreGlobalDefinitions.h"
#include "RenderingCore.h"

namespace eagle {

enum class IndexBufferType{
    UINT_16 = 2,
    UINT_32 = 4
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;
    virtual void * data() = 0;
    virtual size_t size() = 0;
    virtual void upload(void* data, uint32_t size) = 0;
};

}


#endif //EAGLE_INDEXBUFFER_H
