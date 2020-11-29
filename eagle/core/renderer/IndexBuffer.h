//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <vector>
#include "eagle/core/CoreGlobalDefinitions.h"
#include "RenderingCore.h"

EG_BEGIN

enum class IndexBufferType{
    UINT_16 = 2,
    UINT_32 = 4
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;
    virtual void * get_data() = 0;
    virtual size_t get_indices_count() = 0;
    virtual void upload(void* data, uint32_t verticesCount) = 0;
};

EG_END


#endif //EAGLE_INDEXBUFFER_H
