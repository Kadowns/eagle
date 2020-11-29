//
// Created by Novak on 09/06/2019.
//

#ifndef EAGLE_UNIFORMBUFFER_H
#define EAGLE_UNIFORMBUFFER_H

#include "DescriptorItem.h"
#include "Shader.h"

EG_BEGIN

class UniformBuffer : public DescriptorItem {

public:

    explicit UniformBuffer(size_t size) : DescriptorItem(DescriptorType::UNIFORM_BUFFER), m_bytes(size) {}
    virtual ~UniformBuffer() = default;

    virtual void set_data(void *data, size_t size, size_t offset) = 0;
    virtual void push() = 0;
    size_t size() {return m_bytes.size();}
    const std::vector<char>& data() const { return m_bytes; }

protected:
    std::vector<char> m_bytes;
};

EG_END
#endif //EAGLE_UNIFORMBUFFER_H
