#include "eagle/renderer/UniformBufferSet.h"

_EAGLE_BEGIN

UniformBufferSet::UniformBufferSet(const std::vector<std::shared_ptr<UniformBuffer>> &uniformBuffers)
    : m_uniformBuffers(uniformBuffers) {

}

_EAGLE_END