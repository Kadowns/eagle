//
// Created by Novak on 30/07/2019.
//

#ifndef EAGLE_DESCRIPTORITEM_H
#define EAGLE_DESCRIPTORITEM_H

#include "RenderingCore.h"

EG_BEGIN

enum class EG_DESCRIPTOR_TYPE{
    UNIFORM_BUFFER = 0,
    IMAGE_2D = 1
};

class DescriptorItem {
public:
    explicit DescriptorItem(EG_DESCRIPTOR_TYPE type): m_type(type){}
    virtual ~DescriptorItem() {}

    inline EG_DESCRIPTOR_TYPE type() const {return m_type;}

private:
    EG_DESCRIPTOR_TYPE m_type;
};


EG_END

#endif //EAGLE_DESCRIPTORITEM_H
