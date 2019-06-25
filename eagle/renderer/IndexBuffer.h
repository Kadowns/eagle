//
// Created by Novak on 07/06/2019.
//

#ifndef EAGLE_INDEXBUFFER_H
#define EAGLE_INDEXBUFFER_H

#include <vector>
#include "eagle/core/Core.h"

_EAGLE_BEGIN

class IndexBuffer {
public:
    IndexBuffer() = default;
    virtual ~IndexBuffer() = default;

    virtual const std::vector<uint32_t>& get_indices() = 0;
    virtual const void* get_indices_data() = 0;
    virtual uint32_t get_indices_count() = 0;
};

_EAGLE_END


#endif //EAGLE_INDEXBUFFER_H
