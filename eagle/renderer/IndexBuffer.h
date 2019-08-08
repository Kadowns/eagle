//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <vector>
#include "eagle/core/Core.h"
#include "RenderingCore.h"

_EAGLE_BEGIN

enum class INDEX_BUFFER_TYPE{
    UINT_16 = 2,
    UINT_32 = 4
};

class IndexBuffer {
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;
    virtual void * get_data() = 0;
    virtual size_t get_indices_count() = 0;
};

_EAGLE_END


#endif //EAGLE_INDEXBUFFER_H
