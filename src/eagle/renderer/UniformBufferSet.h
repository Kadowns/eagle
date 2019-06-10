//
// Created by Novak on 10/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFERSET_H
#define EAGLE_UNIFORMBUFFERSET_H

#include <vector>

#include "eagle/core/Core.h"
#include "UniformBuffer.h"

_EAGLE_BEGIN

class UniformBufferSet {

public:
    UniformBufferSet(const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers);
    virtual void bind() = 0;

protected:

    std::vector<std::shared_ptr<UniformBuffer>> m_uniformBuffers;

};

_EAGLE_END

#endif //EAGLE_UNIFORMBUFFERSET_H
